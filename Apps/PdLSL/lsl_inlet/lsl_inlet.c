/**************** lsl_inlet ****************/
/* Written by David Medine on behalf of    */
/* Brain Products                          */ 
/* 15/5/2017                               */
/* Released under the GPL                  */
/* This software is free and open source   */
/*******************************************/


#include "m_pd.h"
#include "lsl_c.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pthread.h"

#ifdef _WIN32
#include "windows.h"
#else
#include <unistd.h>
#endif

// pd boilerplate:
static t_class *lsl_inlet_class;

typedef struct _lsl_inlet{

  t_object    x_obj;

  // lsl message buffers
  char*      str_marker;            // only two types allowed (only ones that make sense in Pd)
  float      f_marker;

  // pd outlets
  t_outlet    *symbol_outlet;
  t_outlet    *float_outlet;
  t_outlet    *ts_outlet;
  
  // containers for lsl api
  lsl_inlet               lsl_inlet_obj;      // instantiation of the inlet class
  lsl_streaminfo          lsl_info_list[50];  // to contain list returned by continuous resolver
  int                     lsl_info_list_cnt;
  int                     which;
  lsl_continuous_resolver lsl_cr;
  lsl_channel_format_t    type;
  float                   ts;
  int                     max_buflen;
  int                     max_chunklen;
  double                  lsl_pull_timeout;

  // threading variables for the listen thread and associated data
  pthread_mutex_t listen_lock;
  pthread_t       tid;
  int             stop_;
  int             can_launch_resolver;
  
}t_lsl_inlet;

// for launching the resolve threads
typedef struct _prop_strct{
  t_lsl_inlet *x;
  int          argc;
  t_atom      *argv;
}t_prop_strct;

// listen thread function declaration:
void *lsl_listen_thread(void *in);

// helper function declarations:
void destroy_info_list(t_lsl_inlet *x);
void post_info_list(t_lsl_inlet *x);
int prop_resolve(t_lsl_inlet *x, int argc, t_atom *argv);

// listen thread function:
void *lsl_listen_thread(void *in){


  t_lsl_inlet *x = (t_lsl_inlet *)in;
  
  int ec;
  lsl_channel_format_t type;

  type = lsl_get_channel_format(x->lsl_info_list[x->which]);


  x->stop_ = 0;
  while(x->stop_==0){
    pthread_mutex_lock(&x->listen_lock);
    
    if(type == cft_string)
      x->ts = lsl_pull_sample_str(x->lsl_inlet_obj, &x->str_marker, 1, LSL_FOREVER, &ec);
    else
      x->ts = lsl_pull_sample_f(x->lsl_inlet_obj, &x->f_marker, 1, LSL_FOREVER, &ec);

    pthread_mutex_unlock(&x->listen_lock);
    if(type == cft_string)
      outlet_symbol(x->symbol_outlet,gensym(x->str_marker));
    else
      outlet_float(x->float_outlet, x->f_marker);
      outlet_float(x->ts_outlet, (float)x->ts);

  }
  
  return NULL;
}


// helper functions:
void destroy_info_list(t_lsl_inlet *x){

  int i;

  for(i=0;i<50;i++){
    if(x->lsl_info_list[i] != NULL){
      lsl_destroy_streaminfo(x->lsl_info_list[i]);
      x->lsl_info_list[i] = NULL;
    }
  }
  x->lsl_info_list_cnt=0;
}

void post_info_list(t_lsl_inlet *x){
  
  int i;
  int cnt = (x->lsl_info_list_cnt>=50)?50:x->lsl_info_list_cnt;
  post("----------available lsl streams------------");
  for(i=0;i<cnt;i++){
    post("[%d] name: %s  |  type: %s  |  source_id: %s",
    	 i,
    	 lsl_get_name(x->lsl_info_list[i]),
    	 lsl_get_type(x->lsl_info_list[i]),
    	 lsl_get_source_id(x->lsl_info_list[i]));
  }
}

int prop_resolve(t_lsl_inlet *x, int argc, t_atom *argv){

  t_symbol *arg;
  int resolved_count = 0;
  int i;
  t_atom *outv;

  post("prop_resolve x: %d, argc %d, argv %d", x, argc, argv);
  
  for(i=0;i<50;i++){
    if(x->lsl_info_list[i] == NULL)break;
    else{
      lsl_destroy_streaminfo(x->lsl_info_list[i]);
      x->lsl_info_list[i] = NULL;
    }
  }
  
  x->lsl_info_list_cnt = 0;
  arg = atom_getsymbolarg(0, argc, argv);
  if(!strcmp(arg->s_name, "-name"))
      resolved_count = lsl_resolve_byprop(x->lsl_info_list, 50, "name", atom_getsymbolarg(1,argc,argv)->s_name,0,5);

  else if(!strcmp(arg->s_name, "-type"))
      resolved_count = lsl_resolve_byprop(x->lsl_info_list, 50, "type", atom_getsymbolarg(1,argc,argv)->s_name,0,5);

  else if(!strcmp(arg->s_name, "-source_id"))
      resolved_count = lsl_resolve_byprop(x->lsl_info_list, 50, "source_id", atom_getsymbolarg(1,argc,argv)->s_name,0,5);

  else
    pd_error(x, "lsl_inlet: %s: uniknown flag or argument missing", arg->s_name);

  if(resolved_count!=0)
      return(resolved_count);
  else{
    post("could not find any streams of property %s matching value %s", arg->s_name, atom_getsymbolarg(1,argc,argv)->s_name);
    return 0;
  }
}

// pd methods:
void lsl_inlet_disconnect(t_lsl_inlet *x){
  
  if(x->stop_!=1){
    post("disconnecting from %s stream %s (%s)...",
	 lsl_get_type(x->lsl_info_list[x->which]),
    	 lsl_get_name(x->lsl_info_list[x->which]),
    	 lsl_get_source_id(x->lsl_info_list[x->which]));
    x->stop_=1;
    x->which = -1;
    // here we are forced to call the dreaded pthread_cancel
    // because the lsl_inlet waits forever for a new sample to come in
    // if a stream disappears before the thread exits, it will simply
    // stick waiting for a new sample and joining the thread will
    // halt pd in that state
    pthread_cancel(x->tid);
    // however, we manage all of our resources outside of that thread, so it's cool (I think...)
    
    if(x->lsl_inlet_obj!=NULL){
      lsl_destroy_inlet(x->lsl_inlet_obj);
      x->lsl_inlet_obj=NULL;
    }
    post("...disconnected");
  }
}

void lsl_inlet_connect_by_idx(t_lsl_inlet *x, t_floatarg f){

  int ec;

  if(x->lsl_info_list_cnt==0){
    post("No lsl_info objects available. Please try to resolve available LSL outlets.");
    return;
  }
  else if((f>x->lsl_info_list_cnt)||(f<0)){
      post("Invalid selection from list of available outlets.");
      return;
  }
  else{

    x->which = (int)f;
    post("connecting to %s stream %s (%s)...",
	 lsl_get_type(x->lsl_info_list[x->which]),
    	 lsl_get_name(x->lsl_info_list[x->which]),
	 lsl_get_source_id(x->lsl_info_list[x->which]));
    //if(x->lsl_inlet_obj!=NULL)lsl_destroy_inlet(x->lsl_inlet_obj);
    
    if(lsl_get_channel_format(x->lsl_info_list[x->which])!=cft_string)
      if(lsl_get_channel_format(x->lsl_info_list[x->which])!=cft_float32){
	  pd_error(x, "requested stream has invalid channel format, only strings and floats allowed");
	  return;
	}
    if(lsl_get_nominal_srate(x->lsl_info_list[x->which])!=0){
      	  pd_error(x, "requested stream has invalid nominal sampling rate, this must be 0");
	  return;
    }
    
    x->lsl_inlet_obj = lsl_create_inlet(x->lsl_info_list[x->which], 300, LSL_NO_PREFERENCE,1);
    
    post("...connected, launcing listener thread");
    ec = pthread_create(&x->tid, NULL, lsl_listen_thread, (void *)x);
    if(ec!=0){
      pd_error(x, "Error launching listener thread");
      return;
    }
  }
}

// this works:
void *list_all_thread(void *in){
  
  t_lsl_inlet *x = (t_lsl_inlet *)in;
  int listed_count;
  post("Attempting to find LSL outlets on the network...");
  listed_count = lsl_resolve_all(x->lsl_info_list, 50, 5);
  if(listed_count!=0){
    x->lsl_info_list_cnt = (listed_count>50?50:listed_count);
    post_info_list(x);
  }
  else post("no streams available");
  
  x->can_launch_resolver = 1;
  return NULL;
}
void lsl_inlet_list_all(t_lsl_inlet *x){
  
  int ec;
  pthread_t tid;
  if(x->can_launch_resolver == 0)
    post("LSL outlets cannot be listed at this time. Another process is already at work.");
  else{
    x->can_launch_resolver = 0; // weak locking
    ec = pthread_create(&tid, NULL, list_all_thread, (void *)x);
    if(ec!=0){
      pd_error(x, "Error launching list all thread");
      return;
    }
  }
}

// but for some reason this doesn't work:
/* void *list_by_property_thread(void *in){ */

/*   t_prop_strct *y = (t_prop_strct *)in; */
/*   int listed_count; */
/*   post("thread x: %d, argc %d, argv %d", y->x, y->argc, y->argv); */
/*   listed_count = prop_resolve(y->x, y->argc, y->argv); */
/*   if(listed_count!=0){ */
/*     y->x->lsl_info_list_cnt = (listed_count>50?50:listed_count); */
/*     post_info_list(y->x); */
/*   } */
 
/*   y->x->can_launch_resolver = 1; */
/*   return NULL; */
/* } */
void lsl_inlet_list_by_property(t_lsl_inlet *x, t_symbol *s, int argc, t_atom *argv){

    int listed_count;
    //post("thread x: %d, argc %d, argv %d", y->x, y->argc, y->argv);
  listed_count = prop_resolve(x, argc, argv);
  if(listed_count!=0){
    x->lsl_info_list_cnt = (listed_count>50?50:listed_count);
    post_info_list(x);
  }

  // doesn't work:
  /* int ec; */
  /* pthread_t tid; */
  /* t_prop_strct y; */
  /* post("before pack x: %d, argc %d, argv %d", x, argc, argv); */
  /* y.x = x; */
  /* y.argc = argc; */
  /* y.argv = argv; */
  /* post("after pack x: %d, argc %d, argv %d", y.x, y.argc, y.argv); */
  /* if(x->can_launch_resolver == 0) */
  /*   post("LSL outlets cannot be listed at this time. Another process is already at work."); */
  /* else{ */
  /*   x->can_launch_resolver = 0; // weak locking */
  /*   ec = pthread_create(&tid, NULL, list_by_property_thread, (void *)&y); */
  /*   if(ec!=0){ */
  /*     pd_error(x, "Error launching list by property thread"); */
  /*     return; */
  /*   } */
  /* } */
}
void lsl_inlet_resolve_by_property(t_lsl_inlet *x, t_symbol *s, int argc, t_atom *argv){

  x->lsl_info_list_cnt = 0;
  x->lsl_info_list_cnt = prop_resolve(x, argc, argv);
  if(x->lsl_info_list_cnt != 0)
    lsl_inlet_connect_by_idx(x, 0);
  
}



void *lsl_inlet_new(t_symbol *s, int argc, t_atom *argv){

  int i;
  t_lsl_inlet *x = (t_lsl_inlet *)pd_new(lsl_inlet_class);

  // outlet to forward incoming LSL markers
  x->symbol_outlet = outlet_new(&x->x_obj, &s_symbol);
  // reports the number of markers that came in on this bang
  x->float_outlet = outlet_new(&x->x_obj, &s_float);
  x->ts_outlet = outlet_new(&x->x_obj, &s_float);
  
  x->which = -1;
  x->can_launch_resolver = 1;

  // defaults for the inlet
  x->max_buflen = 100; // 10000 samples

  for(i=0;i<50;i++)
    x->lsl_info_list[i] = NULL;
  x->lsl_info_list_cnt = 0;
  //x->lsl_inlet_obj = NULL;

  x->listen_lock = PTHREAD_MUTEX_INITIALIZER;
  x->stop_=1;
  
  return x;
    
}


void lsl_inlet_free(t_lsl_inlet *x){

  destroy_info_list(x);
  lsl_inlet_disconnect(x);
  if(x->lsl_inlet_obj!=NULL)lsl_destroy_inlet(x->lsl_inlet_obj);

}

void lsl_inlet_setup(void){

  lsl_inlet_class = class_new(gensym("lsl_inlet"),
			      (t_newmethod)lsl_inlet_new,
			      (t_method)lsl_inlet_free,
			      sizeof(t_lsl_inlet),
			      0,
			      A_GIMME,
			      0);

   class_addmethod(lsl_inlet_class,
  		  (t_method)lsl_inlet_disconnect,
  		  gensym("disconnect"),
  		  0);
    
  class_addmethod(lsl_inlet_class,
  		  (t_method)lsl_inlet_connect_by_idx,
  		  gensym("connect_by_idx"),
  		  A_DEFFLOAT,
  		  0);
    
  class_addmethod(lsl_inlet_class,
  		  (t_method)lsl_inlet_list_all,
  		  gensym("list_all"),
  		  0);
  
  class_addmethod(lsl_inlet_class,
  		  (t_method)lsl_inlet_list_by_property,
  		  gensym("list_by_property"),
  		  A_GIMME,
  		  0);
  
  class_addmethod(lsl_inlet_class,
  		  (t_method)lsl_inlet_resolve_by_property,
  		  gensym("resolve_by_property"),
  		  A_GIMME,
  		  0);

  /* class_addbang(lsl_inlet_class, */
  /* 		(t_method)lsl_inlet_bang); */

}









