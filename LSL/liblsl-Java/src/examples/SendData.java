package examples;
import edu.ucsd.sccn.LSL;
import java.io.IOException;

public class SendData {
    public static void main(String[] args) throws IOException, InterruptedException  {
        System.out.println("Creating a new StreamInfo...");
        LSL.StreamInfo info = new LSL.StreamInfo("BioSemi","EEG",8,100,LSL.ChannelFormat.float32,"myuid324457");

        System.out.println("Creating an outlet...");
        LSL.StreamOutlet outlet = new LSL.StreamOutlet(info);
        
        System.out.println("Sending data...");
        float[] sample = new float[8];
        for (int t=0;t<100000;t++) {
            for (int k=0;k<8;k++)
                sample[k] = (float)Math.random()*50-25;
            outlet.push_sample(sample);
            Thread.sleep(10);
        }
        
        outlet.close();
        info.destroy();
    }
}