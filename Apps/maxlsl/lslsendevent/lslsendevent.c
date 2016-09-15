/*
 *  lslsendevent.c
 *  lslsendevent
 *
 *  Created by John Iversen on 1/5/14.
 *
 *  create an event stream and send event strings to it
 *
 *  usage:  [lslsendevent]
 *              sends events to Markers stream named 'MAX_markers'
 *
 *          [lslsendevent streamName]
 *              send event to our own named Markers stream
 */

// Question: any need to 'backdate' the event timestamp to the moment the event was received?
//      It would  account for the latency in internal processing here. Doubtful ths is of consequence.

#include "ext.h"
#include "ext_obex.h"
#include "lsl_c.h"
#include <stdio.h>

#define MAX_EVENT_LENGTH 4096
#define MAX_STREAM_NAME_LENGTH 200
#define DEFAULT_STREAM_NAME "MAX_markers"

//TODO: any need to expose the lsl timestamp of event?

typedef struct _lslsendevent
{
	t_object m_obj;
    
    char eventcode[MAX_EVENT_LENGTH];            /* current event code */
    char lsl_stream_name[MAX_STREAM_NAME_LENGTH];  /* name of stream */
	
	lsl_streaminfo lsl_info;	/* the streaminfo we create */
	lsl_outlet lsl_outlet;		/* a stream outlet to push events to */
 	int lsl_errcode;			/* error code (lsl_lost_error or timeouts) */
    double lsl_timestamp;		/* time stamp of the current sample (in sender time) */

} t_lslsendevent;

static t_class* s_lslsendevent_class = NULL;

void* lslsendevent_new(t_symbol* s, long argc, t_atom* argv);
void  lslsendevent_free(t_lslsendevent* x);
void  lslsendevent_assist(t_lslsendevent* x, void* b, long m, long a, char* s);
void  lslsendevent_bang(t_lslsendevent *x);
void  lslsendevent_anything(t_lslsendevent *x, t_symbol *s, long argc, t_atom *argv);

//DEBUG
void printargs(t_symbol *s, long argc, t_atom *argv)
{
    long i;
    t_atom *ap;
    
    post("message selector is %s",s->s_name);
    post("there are %ld arguments",argc);
    
    // increment ap each time to get to the next atom
    for (i = 0, ap = argv; i < argc; i++, ap++) {
        switch (atom_gettype(ap)) {
            case A_LONG:
                post("%ld: %ld",i+1,atom_getlong(ap));
                break;
            case A_FLOAT:
                post("%ld: %.2f",i+1,atom_getfloat(ap));
                break;
            case A_SYM:
                post("%ld: %s",i+1, atom_getsym(ap)->s_name);
                break;
            default:
                post("%ld: unknown atom type (%ld)", i+1, atom_gettype(ap));
                break;
        }
    }
}
//DEBUG

int main(void)
{
    
    t_class* c = class_new("lslsendevent", (method)lslsendevent_new, (method)lslsendevent_free,
                  sizeof(t_lslsendevent), (method)0L, A_GIMME, 0);
	
	/* Add class methods */
    class_addmethod(c, (method)lslsendevent_bang, "bang", 0);
    class_addmethod(c, (method)lslsendevent_anything, "anything", A_GIMME, 0);
    class_addmethod(c, (method)lslsendevent_assist, "assist", A_CANT, 0);
	
	/* Add attributes */
	
	class_register(CLASS_BOX, c);
	s_lslsendevent_class = c;
	
    return 1;
}

void* lslsendevent_new(t_symbol* s, long argc, t_atom* argv)
{
    
    // DEBUG print out input args
    // printargs(s, argc, argv);
    // DEBUG print out input args
    
	t_lslsendevent* x = (t_lslsendevent*) object_alloc(s_lslsendevent_class);
	
	if (x != NULL) {
        
        post("lslsendevent module for Max/MSP, John Iversen, SCCN, January 2014");
        
        // get event stream name if specified, else use default
        if (argc==1 && argv[0].a_type==A_SYM) {
            strncpy(x->lsl_stream_name, atom_getsym(&argv[0])->s_name, MAX_STREAM_NAME_LENGTH);
        } else {
            strncpy(x->lsl_stream_name, DEFAULT_STREAM_NAME, MAX_STREAM_NAME_LENGTH);
            post(" Using default stream name '%s'",x->lsl_stream_name);
        }
        
        x->eventcode[0]=0; //probably unnecessary--ensure event code is empty to start
				
		post("Creating an event Markers stream named '%s'.",x->lsl_stream_name);
		x->lsl_info = lsl_create_streaminfo(x->lsl_stream_name,"Markers",1,LSL_IRREGULAR_RATE,cft_string,"JRI_Max");
        x->lsl_outlet = lsl_create_outlet(x->lsl_info, 0, 360);
        
        if (x->lsl_outlet) {
            //post("Stream created.\n");
        } else {
            post("Problem creating stream. Events won't be sent.");
        }
	}
	
	return x;
}

void lslsendevent_free(t_lslsendevent* x)
{
	/* Do any deallocation needed here. */
    lsl_destroy_outlet(x->lsl_outlet);
}

// send the event code on a bang
void  lslsendevent_bang(t_lslsendevent *x) {
    
    //post("eventcode length= %d",strlen(x->eventcode));
    
    if (strlen(x->eventcode) && x->lsl_outlet) {
        //post("bang, sending code %s:",x->eventcode);
        char *ptr = x->eventcode;
        lsl_push_sample_str(x->lsl_outlet, &ptr);
    }
}

// store then send first symbol in input out as event string
void  lslsendevent_anything(t_lslsendevent *x, t_symbol *s, long argc, t_atom *argv) {
    strncpy(x->eventcode, s->s_name, MAX_EVENT_LENGTH);
    //post("set event code to: %s",x->eventcode);
    lslsendevent_bang(x);
}

// provide info about in/outlets
void lslsendevent_assist(t_lslsendevent* x, void* b, long m, long a, char* s)
{
	if (m == ASSIST_INLET) {
		switch (a) {
			case 0:
				sprintf(s, "Event string or bang to send last string. Sends to stream named '%s'.",x->lsl_stream_name);
				break;
		}
	} 
	else {
		switch (a) {
			case 0:
				sprintf(s, "Timestamp of event"); //unused: potential for the future
				break;
        }
	}
}