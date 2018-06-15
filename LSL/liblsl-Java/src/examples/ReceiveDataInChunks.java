package examples;
import edu.ucsd.sccn.LSL;

public class ReceiveDataInChunks {
    public static void main(String[] args) throws Exception  {
        System.out.println("Resolving an EEG stream...");
        LSL.StreamInfo[] results = LSL.resolve_stream("type","EEG");

        // open an inlet
        LSL.StreamInlet inlet = new LSL.StreamInlet(results[0]);
        
        // receive data
        float[] sample = new float[10*inlet.info().channel_count()];
        double[] timestamps = new double[10];
        while (true) {
            inlet.pull_chunk(sample,timestamps);
            for (int i=0;i<timestamps.length;i++) {
                for (int k=0;k<sample.length;k++)
                    System.out.print("\t" + Double.toString(sample[k]));
                System.out.println();
            }
        }
    }
}
