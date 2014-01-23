package examples;
import edu.ucsd.sccn.LSL;

public class TestLSL {
    public static void main(String[] args) throws Exception  {
        LSL.ContinuousResolver res = new LSL.ContinuousResolver("type","EEG");
        while (true) {
            LSL.StreamInfo[] results = res.results();
            for (int i=0;i<results.length;i++)
                System.out.print("\t" + results[i].name());
            System.out.println();
            Thread.sleep(250);
        }        
    }
}
