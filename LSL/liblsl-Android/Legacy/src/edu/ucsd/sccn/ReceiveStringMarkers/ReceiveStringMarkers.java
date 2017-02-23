
package edu.ucsd.sccn.ReceiveStringMarkers;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;
import android.os.AsyncTask;
import edu.ucsd.sccn.lsl.*;

public class ReceiveStringMarkers extends Activity
{
    private static lslAndroid lsl;
	
	private static TextView tv;
	 
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

		tv = new TextView(this);
		tv.setText( "Attempting to receive LSL markers: ");
		setContentView(tv);
		

		AsyncTask.execute(new Runnable() {
			public void run() {
			
				vectorinfo results = lsl.resolve_stream("name", "Presentation");
				stream_inlet in = new stream_inlet(results.get(0));
				
				vectorstr sample = new vectorstr(1);
				while(true) {
					in.pull_sample(sample, 1.0);
					final String final_sample = sample.get(0);
					runOnUiThread(new Runnable(){
						@Override
						public void run(){
							tv.setText("Received: " + final_sample);
						}
					});
				}
				
			}
		});
		
       
    }

    static {
        System.loadLibrary("lslAndroid");
    }
}
