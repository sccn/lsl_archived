package ucsd.sccn;

import android.os.Handler;
import ucsd.sccn.lsl.*;

public class SendingThread extends Thread {
	private stream_outlet soutlet;
	private volatile boolean STOP =false;
	private Handler mHandler;
	private int NUM_CHANNEL;
	private int SRATE; //sampling rate
	public SendingThread(int numChannel,int srate, stream_outlet s,Handler handler){
		this.soutlet = s;
		mHandler = handler;
		NUM_CHANNEL = numChannel;
		SRATE = srate; 
	}
	public void run(){
		vectord sample = new vectord(NUM_CHANNEL);
		for (int t = 0; t<Integer.MAX_VALUE && !STOP ; t++) {
			
			//insert a random number into sample vector
			for (int k = 0; k < sample.size(); k++)
				sample.set(k, Math.random()*10);
			
			soutlet.push_sample(sample);
			mHandler.obtainMessage(0, "Sample pushed: "+(t%100)+"\n").sendToTarget();//print some messagess...
			
			try {
				Thread.sleep(1000/SRATE);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		mHandler.obtainMessage(0, "SendThread done....\n").sendToTarget();
	}
	// stop this thread
	public void setSTOP(){
		this.STOP = true;
	}
}
