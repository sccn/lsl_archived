package examples;
import edu.ucsd.sccn.LSL;

public class ReceiveStringMarkers {
    public static void main(String[] args) throws Exception  {
        System.out.println("Resolving a marker stream...");
        LSL.StreamInfo[] results = LSL.resolve_stream("type","Markers");
        LSL.StreamInlet inlet = new LSL.StreamInlet(results[0]);
        
        String[] sample = new String[1];
        while (true) {
            inlet.pull_sample(sample);
            System.out.println(sample[0]);
        }
    }
}
