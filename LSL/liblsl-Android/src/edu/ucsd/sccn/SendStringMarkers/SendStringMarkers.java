
package edu.ucsd.sccn.SendStringMarkers;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;
import android.os.AsyncTask;
import edu.ucsd.sccn.lsl.*;

public class SendStringMarkers extends Activity
{
    private static lslAndroid lsl;
	
	private static TextView tv;
	
	private String markertypes[] = { "Test", "Blah", "Marker", "XXX", "Testtest", "Test-1-2-3" };
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

		tv = new TextView(this);
		tv.setText( "Attempting to send LSL markers: ");
		setContentView(tv);
		

		AsyncTask.execute(new Runnable() {
			public void run() {
				java.util.Random rand = new java.util.Random();
				vectorstr sample = new vectorstr(1);
				stream_info info = new stream_info("MyEventStream", "Markers", 1, lsl.getIRREGULAR_RATE() , channel_format_t.cf_string , "myuniquesourceid23443");
				stream_outlet outlet = new stream_outlet(info);
				
				// send random marker strings
				while (true) {
					// wait for a random period of time
					double endtime = ((double)lsl.local_clock()) + (Math.abs(rand.nextInt()) % 1000) / 1000.0;
					while (((double)lsl.local_clock()) < endtime);
					// and choose the marker to send
					final String mrk = markertypes[Math.abs(rand.nextInt()) % markertypes.length];

					runOnUiThread(new Runnable(){
						@Override
						public void run(){
							tv.setText("Now sending: " + mrk);
						}
					});
					
					sample.set(0,mrk);
					try{
					Thread.sleep(100);
					} catch (Exception ex) {}
					outlet.push_sample(sample);
				}
			}
		});
		
       
    }

    static {
        System.loadLibrary("lslAndroid");
    }
}
