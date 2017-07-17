package com.app.phonesensors;

import ucsd.sccn.R;
import ucsd.sccn.lsl.*;
import android.app.Activity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Vibrator;
import android.view.View;
import android.widget.TextView;

public class PhonesensorActivity extends Activity implements SensorEventListener {

	private float lastX, lastY, lastZ;

	private SensorManager sensorManager;
	private Sensor accelerometer;

	private float deltaXMax = 0;
	private float deltaYMax = 0;
	private float deltaZMax = 0;

	private float deltaX = 0;
	private float deltaY = 0;
	private float deltaZ = 0;

	private float vibrateThreshold = 0;

	private TextView currentX, currentY, currentZ, maxX, maxY, maxZ, lslMessage;

	public Vibrator v;
	
	//LSL
	private stream_info sinfo;
	private stream_outlet soutlet;
	private PhonesensorThread phoneOut ;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_phonesensor);
		initializeViews();

		sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
		if (sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER) != null) {
			// success! we have an accelerometer

			accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
			sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
			vibrateThreshold = accelerometer.getMaximumRange() / 2;
		} else {
			// fai! we dont have an accelerometer!
		}
		
		//initialize vibration
		v = (Vibrator) this.getSystemService(Context.VIBRATOR_SERVICE);
		
		//LSL
		initial();
		phoneOut = new PhonesensorThread(mHandler,soutlet);
		
	}
	public void onPushLSLButtonClick(View view){
		phoneOut.start();
		lslMessage.setText("Pushing to LSL...\n");
	}
	private void initial(){
		WifiManager wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);
		WifiManager.MulticastLock lock = null;

		if (wifi != null) {
			lslMessage.setText("wifi service acquired\n");
			lock = wifi.createMulticastLock("createMulticastLock");
			lock.acquire();
			if (lock != null) {
				lslMessage.append("multicast lock acquired\n");
			}
		}
		
		lslMessage.setText("The LSL clock reads: " + lslAndroid.local_clock() + "\n");
		lslMessage.setText("Creating an outlet..." + "\n");
		sinfo = new stream_info("AndroidPhoneSensorActivity", "EEG", 3, 30,
				channel_format_t.cf_float32, "myuid324457"+System.currentTimeMillis());
		soutlet = new stream_outlet(sinfo);
		lslMessage.setText("LSL is ready");
	}

	public void initializeViews() {
		currentX = (TextView) findViewById(R.id.currentX);
		currentY = (TextView) findViewById(R.id.currentY);
		currentZ = (TextView) findViewById(R.id.currentZ);

		maxX = (TextView) findViewById(R.id.maxX);
		maxY = (TextView) findViewById(R.id.maxY);
		maxZ = (TextView) findViewById(R.id.maxZ);
		
		lslMessage = (TextView)findViewById(R.id.LSLTextView);
	}
	
	private final Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case 0://print message
				lslMessage.setText((String)msg.obj);
				break;

			}
		}
	};
	//onResume() register the accelerometer for listening the events
	protected void onResume() {
		super.onResume();
		sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
	}

	//onPause() unregister the accelerometer for stop listening the events
	protected void onPause() {
		super.onPause();
		sensorManager.unregisterListener(this);
		if(soutlet!=null)
			soutlet.delete();
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		
	}

	@Override
	public void onSensorChanged(SensorEvent event) {

		// clean current values
		displayCleanValues();
		// display the current x,y,z accelerometer values
		displayCurrentValues();
		// display the max x,y,z accelerometer values
		displayMaxValues();

		// get the change of the x,y,z values of the accelerometer
		deltaX = Math.abs(lastX - event.values[0]);
		deltaY = Math.abs(lastY - event.values[1]);
		deltaZ = Math.abs(lastZ - event.values[2]);

		// if the change is below 2, it is just plain noise
		if (deltaX < 2)
			deltaX = 0;
		if (deltaY < 2)
			deltaY = 0;
		if ((deltaZ > vibrateThreshold) || (deltaY > vibrateThreshold) || (deltaZ > vibrateThreshold)) {
			//v.vibrate(50);
		}
		
		//LSL
		if(phoneOut.isAlive()){
			phoneOut.update(deltaX, deltaY, deltaZ);
		}
	}

	public void displayCleanValues() {
		currentX.setText("0.0");
		currentY.setText("0.0");
		currentZ.setText("0.0");
	}

	// display the current x,y,z accelerometer values
	public void displayCurrentValues() {
		currentX.setText(Float.toString(deltaX));
		currentY.setText(Float.toString(deltaY));
		currentZ.setText(Float.toString(deltaZ));
	}

	// display the max x,y,z accelerometer values
	public void displayMaxValues() {
		if (deltaX > deltaXMax) {
			deltaXMax = deltaX;
			maxX.setText(Float.toString(deltaXMax));
		}
		if (deltaY > deltaYMax) {
			deltaYMax = deltaY;
			maxY.setText(Float.toString(deltaYMax));
		}
		if (deltaZ > deltaZMax) {
			deltaZMax = deltaZ;
			maxZ.setText(Float.toString(deltaZMax));
		}
	}
}
