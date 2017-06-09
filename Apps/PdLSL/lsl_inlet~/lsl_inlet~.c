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

//pd boilerplate:
static t_class *lsl_inlet_tilde_class;

typedef struct _lsl_inlet_tilde{

  t_object    x_obj;

  // pd outlets
  t_outlet   **sig_outlets;         // pd multiplexed signal outlet
  int        nout;                  // number of channels
  t_outlet   *ts_outlet;            // timestamp outlet
  t_sample   **lcl_outs;            // for convenience in the processing loop

  t_sample   *sig_buf;              // ring buffer for holding lsl chunks as they arrive
  t_sample   *ts_buf;               // ring buffer for timestamps
  int        buflen;                // length of ring buffer
  int        nchannels;             // number of channels in the lsl_inlet
  int        longbuflen;            // nchannels * buflen (for sig_buf)
  int        widx;                  // write index for ring buffer
  int        ridx;                  // read index for ring buffer
  float      sampling_per;          // time to add to first timestamp to get the chunk stamp
  int        connected;             // flag to say if we are connected or not
  int        ready;                 // flag to say if we have enough samples to start reading
  int        lag;                   // lag time to buffer up

  // do simple upsampling -- this assumes that the incoming sampling rate is less than the audio rate, which it might not be:
  // TODO generalize the resampler (and add filtering ?)
  int       upsample;               // flag to upsample or not
  int       sr_pd;                  // pd's sampling rate
  int       sr_lsl;
  int       L;                      // sr ratio, without decimal places
  int       pL_cnt;                 // counter for the period
  float     rL;                     // remainder
  int       pLplus;                 // number of upsampling periods at which L needs to be increased by 1 (1/rL)
  int       pLplus_cnt;             // counter for when to wait
  
  // containers for lsl api
  lsl_inlet               lsl_inlet_obj;      // instantiation of the inlet class
  lsl_streaminfo          lsl_info_list[50];  // to contain list returned by continuous resolver
  int                     lsl_info_list_cnt;
  int                     which;
  lsl_continuous_resolver lsl_cr;
  lsl_channel_format_t    type;
  float                   ts;
  double                  lsl_pull_timeout;

  // threading variables for the listen thread and associated data
  pthread_mutex_t listen_lock;
  pthread_t       tid;
  int             stop_;
  int             can_launch_resolver;
  
}t_lsl_inlet_tilde;


// for launching the resolve threads
typedef struct _prop_strct{
  
  struct t_lsl_tilde_inlet *x;
  int          argc;
  t_atom      *argv;

}t_prop_strct;


// listen thread function declaration:
static void *lsl_listen_thread(void *in);

// helper function declarations:
static void destroy_info_list(t_lsl_inlet_tilde *x);
static void post_info_list(t_lsl_inlet_tilde *x);
static int prop_resolve(t_lsl_inlet_tilde *x, int argc, t_atom *argv);
static void flush_lsl_buffers(t_lsl_inlet_tilde *x);
static void free_lsl_buffers(t_lsl_inlet_tilde *x);
static void setup_lsl_buffers(t_lsl_inlet_tilde *x);

// perform forward decl:
static t_int *lsl_inlet_tilde_perform(t_int *w);

// listen thread function:
static void *lsl_listen_thread(void *in){


  t_lsl_inlet_tilde *x = (t_lsl_inlet_tilde *)in;
  
  int ec;
  int i;
  lsl_channel_format_t type;
  double ts;
  double *sample_d;
  float *sample_f;
  int *sample_i;

  post("listening");
  sample_d = (double *)t_getbytes(0);
  sample_d = (double *)t_resizebytes(sample_d, 0, sizeof(double)*x->nchannels);
  sample_f = (float *)t_getbytes(0);
  sample_f = (float *)t_resizebytes(sample_f, 0, sizeof(float)*x->nchannels);
  sample_i = (int *)t_getbytes(0);
  sample_i = (int *)t_resizebytes(sample_i, 0, sizeof(int)*x->nchannels);

  type = lsl_get_channel_format(x->lsl_info_list[x->which]);

  x->connected = 1;

  x->stop_ = 0;
  post("%d", type);
  while(x->stop_==0){

    switch(type){

    case cft_float32 :
      ts = lsl_pull_sample_f(x->lsl_inlet_obj, sample_f, x->nchannels, LSL_FOREVER, &ec);
      //post("%d", x->widx);
      for(i=0;i<x->nchannels;i++){
  	x->sig_buf[x->widx*x->nchannels+i] = (t_sample)sample_f[i];
	//post("%f %d\n", sample_f[i], i);
      }
      x->ts_buf[x->widx++] = (t_sample)ts;
      pthread_mutex_lock(&x->listen_lock);
      while(x->widx>=x->buflen)x->widx-=x->buflen;
      while(x->widx<0)x->widx++;
      pthread_mutex_unlock(&x->listen_lock);

      break;
    
    case cft_double64 :
      break;
    
    case cft_int32 :
      break;
    
    
    }

  }

  t_freebytes(sample_d, sizeof(double)*x->nchannels);
  t_freebytes(sample_f, sizeof(float)*x->nchannels);
  t_freebytes(sample_i, sizeof(int)*x->nchannels);
  x->connected = 0;
  return NULL;
}



// helper functions:
static void destroy_info_list(t_lsl_inlet_tilde *x){

  int i;

  for(i=0;i<50;i++){
    if(x->lsl_info_list[i] != NULL){
      lsl_destroy_streaminfo(x->lsl_info_list[i]);
      x->lsl_info_list[i] = NULL;
    }
  }
  x->lsl_info_list_cnt=0;
}

static void post_info_list(t_lsl_inlet_tilde *x){
  
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

static int prop_resolve(t_lsl_inlet_tilde *x, int argc, t_atom *argv){

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
static void lsl_inlet_disconnect(t_lsl_inlet_tilde *x){
  
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
    x->connected = 0;
    x->ready = 0;
  }
}

static void lsl_inlet_connect_by_idx(t_lsl_inlet_tilde *x, t_floatarg f){

  int ec;
  float fL, fpLplus;

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
    
    if(lsl_get_channel_format(x->lsl_info_list[x->which])!=cft_double64)
      if(lsl_get_channel_format(x->lsl_info_list[x->which])!=cft_float32)
	if(lsl_get_channel_format(x->lsl_info_list[x->which])!=cft_int32){
	  pd_error(x, "requested stream has invalid channel format, only floats, doubles, and 32-bit int data allowed");
	  return;
	}
    if(lsl_get_nominal_srate(x->lsl_info_list[x->which])==0){
      pd_error(x, "requested stream has invalid nominal sampling rate, this must not be 0");
      return;
    }

    // prepare the ring buffers based on the stream info
    x->nchannels = lsl_get_channel_count(x->lsl_info_list[x->which]);
    x->longbuflen = x->nchannels * x->buflen;
    //post("%d", x->nchannels);
    if(x->nchannels > x->nout) {
      pd_error(x, "the requested stream has more channels than are available in this pd object");
      return;
    }
    setup_lsl_buffers(x);

    // prepare the upsampling factors based on the stream info
    x->sr_lsl = lsl_get_nominal_srate(x->lsl_info_list[x->which]);
    fL = x->sr_pd/x->sr_lsl;
    x->L = (int)fL;
    x->rL = fL-(float)x->L;
    // this better be an integer or else there will be regular hiccups!
    x->pLplus = (int)(1.0/x->rL); 
    
    x->lsl_inlet_obj = lsl_create_inlet(x->lsl_info_list[x->which], 300, 1, 1);
    
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
  
  t_lsl_inlet_tilde *x = (t_lsl_inlet_tilde *)in;
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
static void lsl_inlet_list_all(t_lsl_inlet_tilde *x){
  
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
static void lsl_inlet_list_by_property(t_lsl_inlet_tilde *x, t_symbol *s, int argc, t_atom *argv){

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

static void lsl_inlet_resolve_by_property(t_lsl_inlet_tilde *x, t_symbol *s, int argc, t_atom *argv){

  x->lsl_info_list_cnt = 0;
  x->lsl_info_list_cnt = prop_resolve(x, argc, argv);
  if(x->lsl_info_list_cnt != 0)
    lsl_inlet_connect_by_idx(x, 0);
  
}



static t_int *lsl_inlet_tilde_perform(t_int *w){
  
  int i,n, rd_pt;
  t_lsl_inlet_tilde *x = (t_lsl_inlet_tilde *)(w[1]);
  t_sample *lcl_ts_out;
  t_sample foo;
  t_sample **lcl_outs = x->lcl_outs;
  
  
  for(i=0;i<x->nout;i++)
    lcl_outs[i] = (t_sample *)w[i+2];
    //x->lcl_outs[i] = (t_sample *)w[i+2];
  lcl_ts_out = (t_sample *)w[i+2];
  n = w[i+3];
    
  //post("tilde perform: ready = %d, lag = %d, connected = %d", x->ready, x->lag, x->connected);
  if(x->connected == 1){
    /* if(x->ready > x->lag){ */
    /*   x->ready = x->lag + 1; // ensure the int doesn't wrap around, there are more elegant ways to do this */
      while(n--){
	rd_pt = x->widx-x->lag;
	if(rd_pt<0)rd_pt+=x->buflen;
	for(i=0;i<x->nout;i++)
	  //if(i<x->nchannels && x->connected==1) {
	    //if(n==0)post("hello");
	  *lcl_outs[i]++ = x->sig_buf[rd_pt*x->nchannels+i];
	*lcl_ts_out++ = x->ts_buf[x->ridx];
	    //}
	//	  else {
	  //*lcl_outs[i]++ = 0.0;
	//*lcl_ts_out++ = 0.0;
	//}
	// check the upsampling to see if it is time to increment the read point
	
	/* if(++x->pL_cnt>=x->L){ */
	/*   if(++x->pLplus_cnt >= x->pLplus) */
	/*     x->pLplus_cnt = 0; */
	
	/* /\*   else{ *\/ */
	/*     x->ridx++; */
	/*     while(x->ridx>=x->buflen)x->ridx-=x->buflen; */
	/*     while(x->ridx<0)x->ridx++; */
	/*     x->pL_cnt = 0; */
	/*   } */
	/* } */
      }
      //}
    /* x->ready+=add; */
  }
  
  return w+x->nout+4;

  
}


static void lsl_inlet_tilde_dsp(t_lsl_inlet_tilde *x, t_signal **sp){

  switch(x->nout){
  case 1:
    dsp_add(lsl_inlet_tilde_perform,4,x,
	    sp[0]->s_vec,
	    sp[1]->s_vec,
	    sp[0]->s_n);
    break;
  case 2:
    dsp_add(lsl_inlet_tilde_perform, 5, x,
	    sp[0]->s_vec,
	    sp[1]->s_vec,
	    sp[2]->s_vec,
	    sp[0]->s_n);
    break;
  case 4:
    dsp_add(lsl_inlet_tilde_perform, 7, x, 
	    sp[0]->s_vec,
	    sp[1]->s_vec,
	    sp[2]->s_vec,
	    sp[3]->s_vec,
	    sp[4]->s_vec,
	    sp[0]->s_n);
    break;
  case 8:
    dsp_add(lsl_inlet_tilde_perform, 11, x,
	    sp[0]->s_vec,
	    sp[1]->s_vec,
	    sp[2]->s_vec,
	    sp[3]->s_vec,
	    sp[4]->s_vec,
	    sp[5]->s_vec,
	    sp[6]->s_vec,
	    sp[7]->s_vec,
	    sp[8]->s_vec,
	    sp[0]->s_n);
    break;
  case 16:
    dsp_add(lsl_inlet_tilde_perform, 19, x,
	    sp[0]->s_vec,
	    sp[1]->s_vec,
	    sp[2]->s_vec,
	    sp[3]->s_vec,
	    sp[4]->s_vec,
	    sp[5]->s_vec,
	    sp[6]->s_vec,
	    sp[7]->s_vec,
	    sp[8]->s_vec,
	    sp[9]->s_vec,
	    sp[10]->s_vec,
	    sp[11]->s_vec,
	    sp[12]->s_vec,
	    sp[13]->s_vec,
	    sp[14]->s_vec,
	    sp[15]->s_vec,
	    sp[16]->s_vec,
	    sp[0]->s_n);
    break;
  case 32:
    dsp_add(lsl_inlet_tilde_perform, 35, x,
	    sp[0]->s_vec,
	    sp[1]->s_vec,
	    sp[2]->s_vec,
	    sp[3]->s_vec,
	    sp[4]->s_vec,
	    sp[5]->s_vec,
	    sp[6]->s_vec,
	    sp[7]->s_vec,
	    sp[8]->s_vec,
	    sp[9]->s_vec,
	    sp[10]->s_vec,
	    sp[11]->s_vec,
	    sp[12]->s_vec,
	    sp[13]->s_vec,
	    sp[14]->s_vec,
	    sp[15]->s_vec,
	    sp[16]->s_vec,
	    sp[17]->s_vec,
	    sp[18]->s_vec,
	    sp[19]->s_vec,
	    sp[20]->s_vec,
	    sp[21]->s_vec,
	    sp[22]->s_vec,
	    sp[23]->s_vec,
	    sp[24]->s_vec,
	    sp[25]->s_vec,
	    sp[26]->s_vec,
	    sp[27]->s_vec,
	    sp[28]->s_vec,
	    sp[29]->s_vec,
	    sp[30]->s_vec,
	    sp[31]->s_vec,
	    sp[32]->s_vec,
	    sp[0]->s_n);
    break;
  }
}

static void flush_lsl_buffers(t_lsl_inlet_tilde *x){

  int i,j;
  if(x->sig_buf!=0)memset(x->sig_buf, 0.0, x->longbuflen * sizeof(t_sample));
    /* for(i=0;i<x->buflen*x->nchannels;i++) */
    /*   x->sig_buf[i] =  0.0; */
  if(x->ts_buf!=0)memset(x->ts_buf, 0.0, x->buflen * sizeof(t_sample));
    /* for(i=0;i<x->buflen*x->nchannels;i++) */
    /*   x->ts_buf[i] = 0.0; */

  x->widx = 0;
  x->ridx = 0;
  x->ready = 0;
  
}

static void free_lsl_buffers(t_lsl_inlet_tilde *x){

  int i;
  post("in the free method");
  if(x->sig_buf!=0){
    post("inside sig_buf");
    t_freebytes(x->sig_buf, x->longbuflen * sizeof(float));
  }
  if(x->ts_buf!=0)
    t_freebytes(x->ts_buf, x->buflen * sizeof(float));

}

static void setup_lsl_buffers(t_lsl_inlet_tilde *x){

  int i;
  post("%f", x->sig_buf);
  if(x->sig_buf!=0)free_lsl_buffers(x);
  
  x->sig_buf = (t_sample *)t_getbytes(0);
  x->sig_buf = (t_sample *)t_resizebytes(x->sig_buf, 0, sizeof(float) * x->longbuflen);

  x->ts_buf = (t_sample *)t_getbytes(0);
  x->ts_buf = (t_sample *)t_resizebytes(x->ts_buf, 0, sizeof(float) * x->buflen);
  //flush_lsl_buffers(x);
}

static void *lsl_inlet_tilde_new(t_symbol *s, int argc, t_atom *argv){

  int i, lcl_nout;
  t_symbol *firstarg;
  t_lsl_inlet_tilde *x = (t_lsl_inlet_tilde *)pd_new(lsl_inlet_tilde_class);

  // defaults
  x->nout = 8;
  x->buflen = 10*sys_getblksize();
  x->connected = 0;

  x->widx = 0;
  x->ridx = 0;
  x->ready = 0;
  x->lag = sys_getblksize();
  x->sr_pd = sys_getsr();
  x->L=0;
  x->rL=0.0;
  x->pLplus = 0;
  x->pLplus_cnt = 0;
  x->upsample = 1;

  // parse creation args
  while (argc>0){
   firstarg = atom_getsymbolarg(0,argc, argv);
   if(!strcmp(firstarg->s_name, "-lag")) {
      
      x->lag = sys_getblksize() * atom_getfloatarg(1, argc, argv);
      argc-=2;
      argv+=2;
    }

    else if(!strcmp(firstarg->s_name, "-buflen")){
      if(atom_getfloatarg(1, argc, argv) == 0)
	x->buflen = 1;
      else{
	x->buflen = sys_getblksize() * atom_getfloatarg(1, argc, argv);
	argc-=2;
	argv+=2;
      }
    }
    
    else if(!strcmp(firstarg->s_name, "-nout")){
      lcl_nout = (atom_getfloatarg(1, argc, argv));
      if((lcl_nout==1) || (lcl_nout==2) || (lcl_nout==4) ||
	 (lcl_nout!=8) || (lcl_nout!=16) || (lcl_nout!=32)){
	 
  	    x->nout = lcl_nout;

      }
      else
	post("Invalid outlet selection (must be 1, 2, 4, 8, 16, or 32): reverting to 8");
    
      argc-=2;
      argv+=2;
    }
    else {
      pd_error(x, "%s: unkown flag or argument missing",
  	       firstarg->s_name);
      argc--, argv++;
    }
  }

  // allocate these once we connect to the outlet
  x->sig_buf = 0;
  x->ts_buf  = 0;
  x->nchannels = 0; // this gets set on inlet creation
  
  // setup the memory
  x->lcl_outs = (t_sample **)t_getbytes(0);
  x->lcl_outs = (t_sample **)t_resizebytes(x->lcl_outs, 0, sizeof(t_sample *) * x->nout);

  // setup outlets
  x->sig_outlets = (t_outlet **)t_getbytes(0);
  x->sig_outlets = (t_outlet **)t_resizebytes(x->lcl_outs, 0, sizeof(t_sample *) * x->nout);
  for(i=0;i<x->nout;i++) x->sig_outlets[i] = outlet_new(&x->x_obj, &s_signal);
  x->ts_outlet = outlet_new(&x->x_obj, &s_signal);

  x->which = -1;
  x->can_launch_resolver = 1;


  for(i=0;i<50;i++)
    x->lsl_info_list[i] = NULL;
  x->lsl_info_list_cnt = 0;
  //x->lsl_inlet_obj = NULL;

  x->listen_lock = PTHREAD_MUTEX_INITIALIZER;
  x->stop_=1;
  
  return x;
    
}



static void lsl_inlet_tilde_free(t_lsl_inlet_tilde *x){

  int i;
  
  destroy_info_list(x);
  lsl_inlet_disconnect(x);
  if(x->lsl_inlet_obj!=NULL)lsl_destroy_inlet(x->lsl_inlet_obj);

  if(x->lcl_outs!=0)
    t_freebytes(x->lcl_outs, sizeof(t_sample *)*x->nout);

  if(x->sig_outlets!=0)
    t_freebytes(x->sig_outlets, sizeof(t_outlet *)*x->nout);

  free_lsl_buffers(x);
  
}

void lsl_inlet_tilde_setup(void){

  lsl_inlet_tilde_class = class_new(gensym("lsl_inlet~"),
				    (t_newmethod)lsl_inlet_tilde_new,
				    (t_method)lsl_inlet_tilde_free,
				    sizeof(t_lsl_inlet_tilde),
				    0,
				    A_GIMME,
				    0);

   class_addmethod(lsl_inlet_tilde_class,
  		  (t_method)lsl_inlet_disconnect,
  		  gensym("disconnect"),
  		  0);
    
  class_addmethod(lsl_inlet_tilde_class,
  		  (t_method)lsl_inlet_connect_by_idx,
  		  gensym("connect_by_idx"),
  		  A_DEFFLOAT,
  		  0);
    
  class_addmethod(lsl_inlet_tilde_class,
  		  (t_method)lsl_inlet_list_all,
  		  gensym("list_all"),
  		  0);
  
  class_addmethod(lsl_inlet_tilde_class,
  		  (t_method)lsl_inlet_list_by_property,
  		  gensym("list_by_property"),
  		  A_GIMME,
  		  0);
  
  class_addmethod(lsl_inlet_tilde_class,
  		  (t_method)lsl_inlet_resolve_by_property,
  		  gensym("resolve_by_property"),
  		  A_GIMME,
  		  0);

  class_addmethod(lsl_inlet_tilde_class,
		  (t_method)lsl_inlet_tilde_dsp, gensym("dsp"), 0);

  /* class_addbang(lsl_inlet_class, */
  /* 		(t_method)lsl_inlet_bang); */

}









