package examples;
import edu.ucsd.sccn.LSL;
import java.io.IOException;

public class SendStringMarkers {
    public static void main(String[] args) throws IOException, InterruptedException  {
        System.out.println("Creating a new StreamInfo...");
        LSL.StreamInfo info = new LSL.StreamInfo("MyMarkers","Markers",1,LSL.IRREGULAR_RATE,LSL.ChannelFormat.string,"myuid4563");

        System.out.println("Creating an outlet...");
        LSL.StreamOutlet outlet = new LSL.StreamOutlet(info);
        
        System.out.println("Sending data...");
        String[] strings = {"Test 1-2-3","Blah","ABC"};
        String[] sample = new String[1];
        for (int t=0;t<100000;t++) {
            sample[0] = strings[t%3];
            outlet.push_sample(sample);
            Thread.sleep((long)(Math.random()*2000));
        }
        
        outlet.close();
        info.destroy();
    }
}