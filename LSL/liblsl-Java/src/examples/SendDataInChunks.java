package examples;
import edu.ucsd.sccn.LSL;
import java.io.IOException;

public class SendDataInChunks {
    public static void main(String[] args) throws IOException, InterruptedException  {
        System.out.println("Creating a new StreamInfo...");
        LSL.StreamInfo info = new LSL.StreamInfo("BioSemi","EEG",8,100,LSL.ChannelFormat.float32,"myuid324457");

        System.out.println("Creating an outlet...");
        LSL.StreamOutlet outlet = new LSL.StreamOutlet(info);
        
        System.out.println("Sending data...");
        float[] chunk = new float[10*8];
        for (int t=0;t<100000;t++) {
            // the chunk array contains first all values for the first sample, then the second, and so on
            for (int k=0;k<chunk.length;k++)
                chunk[k] = (float)Math.random()*50-25;
            outlet.push_chunk(chunk); // note: it is also possible to pass in time stamps
            Thread.sleep(100);
        }
        
        outlet.close();
        info.destroy();
    }
}
