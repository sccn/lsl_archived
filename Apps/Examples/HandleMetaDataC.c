#include <lsl_c.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	int c;							/* channel index */
	lsl_xml_ptr desc,chns,chn,cap;	/* some xml element pointers */ 
	lsl_outlet outlet;				/* stream outlet */
	lsl_streaminfo info,inf;		/* streaminfo objects */
	lsl_inlet inlet;				/* a stream inlet to get samples from */
	int errcode;					/* error code (lsl_lost_error or timeouts) */
	const char* labels[] = {"C3","C4","Cz","FPz","POz","CPz","O1","O2"};

	/* create a new streaminfo and declare some meta-data (in accordance with XDF format) */
	const char* name = argc > 1 ? argv[1] : "MetaTester";
	info = lsl_create_streaminfo(name,"EEG",8,100,cft_float32,"myuid323457");
	desc = lsl_get_desc(info);
	chns = lsl_append_child(desc,"channels");
	for (c=0;c<8;c++) {
		chn = lsl_append_child(chns,"channel");
		lsl_append_child_value(chn,"label",labels[c]);
		lsl_append_child_value(chn,"unit","microvolts");
		lsl_append_child_value(chn,"type","EEG");
	}
	lsl_append_child_value(desc,"manufacturer","SCCN");
	cap = lsl_append_child(desc,"cap");
    lsl_append_child_value(cap,"name","EasyCap");
    lsl_append_child_value(cap,"size","54");
    lsl_append_child_value(cap,"labelscheme","10-20");

	/* create outlet for the stream */
	outlet = lsl_create_outlet(info,0,360);

	/*  === the following could run on another computer === */

	/* resolve the stream and open an inlet */
	lsl_resolve_byprop(&info,1,"name","MetaTester",1,LSL_FOREVER);
	inlet = lsl_create_inlet(info, 360, LSL_NO_PREFERENCE, 1);
	inf = lsl_get_fullinfo(inlet,LSL_FOREVER,&errcode);
	printf("The stream's XML meta-data is: \n");
	printf("%s\n",lsl_get_xml(inf)); 
	printf("The manufacturer is: %s\n",lsl_child_value_n(lsl_get_desc(inf),"manufacturer"));
	printf("The cap circumference is: %s\n",lsl_child_value_n(lsl_child(lsl_get_desc(inf),"cap"),"size"));
	printf("The channel labels are as follows:\n");
	chn = lsl_child(lsl_child(lsl_get_desc(inf),"channels"),"channel");
    for (c=0; c<lsl_get_channel_count(inf); c++) {
		printf("  %s\n",lsl_child_value_n(chn,"label"));
        chn = lsl_next_sibling(chn);
    }

	/* destroy objects and free memory */
	lsl_destroy_streaminfo(inf);
	lsl_destroy_inlet(inlet);
	lsl_destroy_outlet(outlet);
	lsl_destroy_streaminfo(info);

	/* wait for keypress */
	getchar();
	return 0;
}
