/**************** lsl_outlet ***************/
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

#ifdef _WIN32
#include "windows.h"
#else
#include <unistd.h>
#endif

// pd boilerplate:
static t_class *lsl_outlet_class;

typedef struct _lsl_outlet{

  t_object    x_obj;
  t_outlet    *f_outlet;
 
  lsl_outlet              lsl_outlet_obj;      // instantiation of the outlet class           
  lsl_channel_format_t    cft;
  
  float                   outlet_established;
  
}t_lsl_outlet;

// helper function declarations:
int parse_and_create_outlet(t_lsl_outlet *x, int argc, t_atom *argv);

// helper function definitions
int parse_and_create_outlet(t_lsl_outlet *x, int argc, t_atom *argv){

  int res = 1;
  lsl_streaminfo info;
  char stream_name[100];
  char stream_type[100];
  char stream_format[100];
  int srate = 0;
  int channel_format;
  char source_id[100];

  t_symbol *arg1, *arg2;

  strcpy(stream_name, "\n");
  strcpy(stream_format, "\n");              // this is all that is allowed here
  strcpy(source_id, "\n");                  // this is optional, but needed in the c api so default is empty
  strcpy(stream_type, "pd_lsl");            // also optional
  
  while(argc > 0) {
    t_symbol *arg1 = atom_getsymbolarg(0, argc, argv);
    if(!strcmp(arg1->s_name, "-name"))
      {
	arg2 = atom_getsymbolarg(1,argc,argv);
  	strcpy(stream_name, arg2->s_name);
  	argc-=2, argv+=2;
	res*=2;
      }

    else if(!strcmp(arg1->s_name, "-type"))
      {
	arg2 = atom_getsymbolarg(1,argc,argv);
  	strcpy(stream_type, arg2->s_name);
  	argc-=2, argv+=2;
	res*=2;
      }
    
    else if(!strcmp(arg1->s_name, "-format"))
      {
	arg2 = atom_getsymbolarg(1,argc,argv);
  	strcpy(stream_format, arg2->s_name);
	if(!strcmp(stream_format, "cft_float32")){res*=2;channel_format = cft_float32;x->cft=cft_float32;}
	else if(!strcmp(stream_format, "cft_string")){res*=2;channel_format = cft_string;x->cft=cft_string;}
	else {pd_error(x, "invalid channel type: %s", stream_format); return -1;}
  	argc-=2, argv+=2;
      }
    
    else if(!strcmp(arg1->s_name, "-id"))
      {
	arg2 = atom_getsymbolarg(1,argc,argv);
  	strcpy(source_id, arg2->s_name);
  	argc-=2, argv+=2;
      }
    
 
    else{
      pd_error(x, "lsl_inlet: %s: unknown flag or argument missing", arg1->s_name);
      argc--;
      argv++;
    }
  }

  // we only go if we got enough correct input parameters
  if(res==8){
    info = lsl_create_streaminfo(stream_name, stream_type, 1, 0, channel_format, source_id);
    x->lsl_outlet_obj = lsl_create_outlet(info, 0, 360);
    res = 0;
  }
  else res = -1; // failed

  return res;
}

// lsl methods
void lsl_outlet_create_outlet(t_lsl_outlet *x, t_symbol *s, int argc, t_atom *argv){
  int res;
  x->outlet_established = 0;
  if(x->lsl_outlet_obj!=NULL){
    lsl_destroy_outlet(x->lsl_outlet_obj);
    x->lsl_outlet_obj=NULL;
  }
  res = parse_and_create_outlet(x, argc, argv);
  if(res==0)x->outlet_established = 1;
  else x->outlet_established = 0;
  outlet_float(x->f_outlet, x->outlet_established);
}

void lsl_outlet_push_str(t_lsl_outlet *x, t_symbol *s, int argc, t_atom *argv){

  char *str;
  int ec;
  
  if(x->outlet_established!=1)return;
  if(x->cft!=cft_string){
    post("invalid argument to push_str");
    return;
  }

  str = atom_getsymbolarg(0, argc, argv)->s_name;
  ec=lsl_push_sample_str(x->lsl_outlet_obj, &str);

}

void lsl_outlet_push_f(t_lsl_outlet *x, t_floatarg f){

  int ec;
  if(x->outlet_established!=1)return;
  if(x->cft!=cft_float32){
    post("invalid argument to push_f");
    return;
  }
  ec=lsl_push_sample_f(x->lsl_outlet_obj, &f);
}

void lsl_outlet_destroy_outlet(t_lsl_outlet *x){

  if(x->lsl_outlet_obj!=NULL){
    lsl_destroy_outlet(x->lsl_outlet_obj);
    x->lsl_outlet_obj = NULL;
    x->outlet_established=0;
  }
  outlet_float(x->f_outlet, x->outlet_established);

}

void lsl_outlet_is_outlet_established(t_lsl_outlet *x){

  outlet_float(x->f_outlet, x->outlet_established);
}

void *lsl_outlet_new(t_symbol *s, int argc, t_atom *argv){

  int i;
  t_lsl_outlet *x = (t_lsl_outlet *)pd_new(lsl_outlet_class);

  x->f_outlet = outlet_new(&x->x_obj, &s_float);
  
  x->lsl_outlet_obj = NULL;
  x->outlet_established = 0;

  if(argc>0)lsl_outlet_create_outlet(x, s, argc, argv);
  return x;
    
}


void lsl_outlet_free(t_lsl_outlet *x){

  if(x->lsl_outlet_obj!=NULL)lsl_destroy_outlet(x->lsl_outlet_obj);
}

void lsl_outlet_setup(void){

  lsl_outlet_class = class_new(gensym("lsl_outlet"),
			      (t_newmethod)lsl_outlet_new,
			      (t_method)lsl_outlet_free,
			      sizeof(t_lsl_outlet),
			      0,
			      A_GIMME,
			      0);

  class_addmethod(lsl_outlet_class,
		  (t_method)lsl_outlet_create_outlet,
		  gensym("create_outlet"),
		  A_GIMME,
		  0);

  class_addmethod(lsl_outlet_class,
  		  (t_method)lsl_outlet_push_str,
  		  gensym("push_str"),
  		  A_GIMME,
  		  0);

  class_addmethod(lsl_outlet_class,
  		  (t_method)lsl_outlet_push_f,
  		  gensym("push_f"),
  		  A_DEFFLOAT,
  		  0);

  class_addmethod(lsl_outlet_class,
		  (t_method)lsl_outlet_destroy_outlet,
		  gensym("destroy_outlet"),
		  0);

  class_addmethod(lsl_outlet_class,
		  (t_method)lsl_outlet_is_outlet_established,
		  gensym("is_outlet_established"),
		  0);

}



























