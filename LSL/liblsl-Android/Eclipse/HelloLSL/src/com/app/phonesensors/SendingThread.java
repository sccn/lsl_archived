package com.app.phonesensors;

import android.os.Handler;
import ucsd.sccn.lsl.*;

public class SendingThread extends Thread {
	private stream_outlet soutlet;
	private boolean STOP =false;
	private Handler mHandler;
	public SendingThread(stream_outlet s,Handler handler){
		this.soutlet = s;
		mHandler = handler;
	}
	//testing 
	public void run(){
		vectord sample = new vectord(8);//[8];
		do{
			for (int t = 0; t < 100*60; t++) {
				for (int k = 0; k < sample.size(); k++)
					sample.set(k, Math.random()*50);//Math.sin(2*Math.PI*t/100));
				soutlet.push_sample(sample);
				mHandler.obtainMessage(0, "Sample pushed: "+(t%100)+"\n").sendToTarget();
				try {
					Thread.sleep(100);//100hz
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}while(!STOP && soutlet!=null);
		mHandler.obtainMessage(0, "SendThread done....\n").sendToTarget();
	}
	public void run_(){
		while(!STOP){
			try{
				if(soutlet!=null){
					//soutlet.push_sample();
				}
				sleep(100);
			}catch(Exception e){

			}
		}
	}
	public void setSTOP(){
		this.STOP = true;
	}
}
