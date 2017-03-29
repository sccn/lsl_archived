package examples;
import edu.ucsd.sccn.LSL;

public class ReceiveData {
    public static void main(String[] args) throws Exception  {
		try {
			System.out.println("Resolving an EEG stream...");
			LSL.StreamInfo[] results = LSL.resolve_stream("type","EEG");

			// open an inlet
			LSL.StreamInlet inlet = new LSL.StreamInlet(results[0]);
			
			// receive data
			float[] sample = new float[inlet.info().channel_count()];
			while (true) {
				inlet.pull_sample(sample);
				for (int k=0;k<sample.length;k++)
					System.out.print("\t" + Double.toString(sample[k]));
				System.out.println();
			}
		} catch(Exception ex) {
			ex.printStackTrace();
		}
	}
}
