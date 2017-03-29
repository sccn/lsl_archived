package edu.ucsd.sccn.receivestringmarkers;
import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;
import android.os.AsyncTask;

import java.io.IOException;

import edu.ucsd.sccn.LSL;

public class MainActivity extends Activity
{

    private static TextView tv;

    void showMessage(String string) {
        final String finalString = string;
        runOnUiThread(new Runnable(){
            @Override
            public void run(){
                tv.setText(finalString);
            }
        });
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        tv = new TextView(this);
        tv.setText( "Attempting to send LSL markers: ");
        setContentView(tv);



        AsyncTask.execute(new Runnable() {
            public void run() {
                LSL.StreamInlet inlet = null;
                try {

                    showMessage("Resolving a marker stream...");
                    LSL.StreamInfo[] results = LSL.resolve_stream("type","Markers");
                    inlet = new LSL.StreamInlet(results[0]);
                    showMessage("Inlet opened");

                } catch(IOException ex) {
                    showMessage("Unable to open LSL inlet. Have you added <uses-permission android:name=\"android.permission.INTERNET\" /> to AndroidManifest.xml?");
                    return;
                }

                String[] sample = new String[1];
                try{
                while (true) {
                        inlet.pull_sample(sample);
                        final String finalSample = sample[0];
                        System.out.println(sample);

                        showMessage("Received: " + finalSample);

                    }
                } catch (Exception ex) {
                    showMessage(ex.getMessage());
                    inlet.close();

                }


            }
        });


    }

}

