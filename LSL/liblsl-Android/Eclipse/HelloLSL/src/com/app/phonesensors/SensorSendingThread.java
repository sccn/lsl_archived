package com.app.phonesensors;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import ucsd.sccn.lsl.*;

public class SensorSendingThread  extends Thread implements SensorEventListener {
	private stream_outlet soutlet;
	private boolean STOP =false;
	private Handler mHandler;

	private SensorManager sensorManager;
	private Sensor accelerometer;

	private vectord accelerometerSamples ;
	private boolean sensorChanged;


	private float deltaX = 0;
	private float deltaY = 0;
	private float deltaZ = 0;
	
	private float lastX = 0;
	private float lastY = 0;
	private float lastZ = 0;

	public SensorSendingThread(stream_outlet s,Handler handler, SensorManager sManager){
		this.soutlet = s;
		mHandler = handler;
		accelerometerSamples= new vectord(3);//3ch
		sensorChanged = false;
		sensorManager = sManager;

		if (sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER) != null) {
			// success! we have an accelerometer

			accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
			sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
			mHandler.obtainMessage(0, "OK, I get Android's Accelerometer.....\n").sendToTarget();

		} else {
			mHandler.obtainMessage(0, "I can't get Android's Accelerometer.....\n").sendToTarget();
		}
	}
	//testing 
	public void run(){
		//vectord sample = new vectord(8);//[8];
		do{
			//for (int t = 0; t < 100*60; t++) {
			//	for (int k = 0; k < sample.size(); k++)
			//		sample.set(k, Math.random()*50);//Math.sin(2*Math.PI*t/100));
			if(sensorChanged){
				soutlet.push_sample(accelerometerSamples);
				sensorChanged = false;
				mHandler.obtainMessage(0, "Sensor pushed: \n").sendToTarget();
			}
			try {
				Thread.sleep(10);//100hz
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			//}
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
		sensorManager.unregisterListener(this);

	}
	@Override
	public void onSensorChanged(SensorEvent event) {
		// TODO Auto-generated method stub
		deltaX = Math.abs(lastX - event.values[0]);
		deltaY = Math.abs(lastY - event.values[1]);
		deltaZ = Math.abs(lastZ - event.values[2]);
		if (deltaX > 2 && deltaY >2 && deltaZ>2){
			accelerometerSamples.set(0, event.values[0]);
			accelerometerSamples.set(1, event.values[1]);
			accelerometerSamples.set(2, event.values[2]);
			lastX = event.values[0];
			lastY = event.values[1];
			lastZ = event.values[2];
			sensorChanged = true;
		}
	}
	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub


	}
}