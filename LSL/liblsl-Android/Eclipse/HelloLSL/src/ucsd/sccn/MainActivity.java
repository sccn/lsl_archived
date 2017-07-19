package ucsd.sccn;
import ucsd.sccn.lsl.*;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity implements View.OnClickListener{
	//GUI
	private Button exitBtn, startBtn, stopBtn;
	private TextView lslMessage;

	//LSL
	private stream_info sinfo;
	private stream_outlet soutlet;
	private int NUM_CHANNEL = 3;//number of channels 
	private int SAMPLE_RATE = 60;//sampling rate
	private SendingThread lsl;//independent thread to push random numbers into LSL stream
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main_layout);
		startBtn = (Button)findViewById(R.id.buttonStart);
		startBtn.setOnClickListener(this);

		stopBtn = (Button)findViewById(R.id.buttonStop);
		stopBtn.setOnClickListener(this);

		exitBtn = (Button)findViewById(R.id.buttonExit);
		exitBtn.setOnClickListener(this);

		lslMessage = (TextView)findViewById(R.id.textView1);

		//initial LSL setting
		initialLSL();

		showLicenseAgreement();

	}

	// load the compiled package into Android
	static {
		System.loadLibrary("lslAndroid");
	}
	
	private void initialLSL(){
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
		try{
			lslMessage.append("The LSL clock reads: " + lslAndroid.local_clock() + "\n");
			lslMessage.append("Creating an outlet..." + "\n");
			sinfo = new stream_info("AndroidRandomNumberActivity", "EEG", NUM_CHANNEL, SAMPLE_RATE,
					channel_format_t.cf_float32, "myuid"+System.currentTimeMillis());
			soutlet = new stream_outlet(sinfo);
			lslMessage.append("LSL is ready");
		}
		catch (Exception e){
			Log.e("Exception in initialLSL: ", e.getMessage());
		}
	}
	// a call back function writes info passed from SendingThread
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
	
	// enable or disable the SendingThread that pushes random data into LSL stream
	@Override
	public void onClick(View arg0) {
		if(R.id.buttonStart == arg0.getId()){
			lsl = new SendingThread(NUM_CHANNEL, SAMPLE_RATE, soutlet,mHandler);
			lsl.start();
		}
		else if(R.id.buttonStop == arg0.getId()){
			lsl.setSTOP();
		}
		if(R.id.buttonExit == arg0.getId()){
			showExitDialog();
		}

	}
	private void showExitDialog(){
		AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);
		alertDialogBuilder.setTitle("Questions? Please mail to yute@sccn.ucsd.edu. Thank you!!");
		alertDialogBuilder
		.setCancelable(false)
		.setPositiveButton("Yes",new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,int id) {
				MainActivity.this.onDestroy();
			}
		})
		.setNegativeButton("No",new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,int id) {
				dialog.cancel();
			}
		});
		AlertDialog alertDialog = alertDialogBuilder.create();
		alertDialog.show();
	}
	private void showLicenseAgreement(){
		AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);
		// set title
		alertDialogBuilder.setTitle("User License Agreement");
		alertDialogBuilder.setMessage("2017/01/17, Yute Wang, SCCN, UCSD."
				+ "\r\n\r\n"
				+ "Please read this License Agreement (Agreement) carefully before clicking the 'I Agree' "
				+ "button, downloading or using this application (Application). \r\n"
				+ "By clicking the 'I Agree' button, downloading or using the Application, you are agreeing to be bound "
				+ "by the terms and conditions of this Agreement. "
				+ "If you do not agree to the terms of this Agreement, do not click on the 'I Agree' button and do not "
				+ "download or use the Application."
				+ "\r\n\r\n"
				+ "--License-- \r\n"
				+ "Yute Wang (YT) grants you a revocable, non­exclusive, non­transferable, limited license "
				+ "to download, install and use the Application solely for your personal, non­commercial purposes "
				+ "strictly in accordance with the terms of this Agreement. "
				+ "\r\n\r\n"
				+ "--Restrictions-- \r\n"
				+ "You agree not to, and you will not permit others to "
				+ "modify, license, sell, rent, lease, assign, distribute, transmit, "
				+ "host, outsource, disclose or otherwise commercially exploit the Application or make the Application available to any third party. "
				+ "\r\n\r\n"
				+ "--Modifications to Application-- \r\n"
				+ "YT reserves the right to modify, suspend or discontinue, temporarily or "
				+ "permanently, the Application or any service to which it connects, with or without notice and without liability to you. "
				+ "\r\n\r\n"
				+ "--Term and Termination--\r\n"
				+ "This Agreement shall remain in effect until terminated by YT. "
				+ "YT may, in its sole discretion, at any time and for any or no reason, "
				+ "suspend or terminate this Agreement with or without prior notice. "
				+ "This Agreement will terminate immediately, without prior notice from YT, in "
				+ "the event that you fail to comply with any provision of this Agreement. You may also terminate this "
				+ "Agreement by deleting the Application and all copies thereof from your mobile device or from your "
				+ "desktop. Upon termination of this Agreement, you shall cease all use of the Application and delete all copies "
				+ "of the Application from your mobile device or from your desktop.");

		alertDialogBuilder
		.setCancelable(false)
		.setPositiveButton("I Don't Agree",new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,int id) {
				MainActivity.this.finish();
			}
		})
		.setNegativeButton("I Agree",new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,int id) {
				dialog.cancel();
			}
		});
		AlertDialog alertDialog = alertDialogBuilder.create();
		alertDialog.show();
	}
	
	// close this activity
	public void onDestroy() {
		super.onDestroy();
		finish();
	}
}
