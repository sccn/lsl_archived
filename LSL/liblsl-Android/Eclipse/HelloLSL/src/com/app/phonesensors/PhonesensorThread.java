package com.app.phonesensors;


import java.util.Vector;

import ucsd.sccn.lsl.*;

import android.os.Handler;

public class PhonesensorThread extends Thread {
	private Vector buffer = new Vector<vectord>();
	private vectorf sample = new vectorf(3);
	private vectorf temp = new vectorf(3);
	private int addIndex=0, removeIndex=0;

	private Handler handler;
	private stream_outlet outlet;
	public PhonesensorThread(Handler mHandler, stream_outlet out){
		outlet = out;
		handler = mHandler;
	}
	public void run(){
		try{
			while(true){
				if(buffer.size()>10){
					outlet.push_sample((vectorf) buffer.get(0));
					buffer.remove(0);	
					handler.obtainMessage(0, "Pushing...\n").sendToTarget();
					}
				else{
					handler.obtainMessage(0, "Size = "+buffer.size()+"...\n").sendToTarget();
					sleep(10);
				}
			}
		}catch(Exception e){

		}
	}
	public void update(float xx, float yy, float zz){
		sample.set(0, xx);
		sample.set(1, yy);
		sample.set(2, zz);
		buffer.add(sample);
	}
}
