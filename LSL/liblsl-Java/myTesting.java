import java.io.*;

public class myTesting{
    private static liblsl lsl;
    private static stream_info sinfo;
    private static stream_inlet sinlet;
    private static vectorinfo vector_info;
    private static vectorf vector;
    
    public static void main(String[] args){
        System.out.println("Testing begin:\n");

		// load the right wrapper library...
	    if (System.getProperty("os.name").startsWith("Windows")) {
			if (System.getProperty("sun.arch.data.model").equals("32")) {
				System.load(System.getProperty("user.dir") + "/liblslJNI32.dll");
			} else {
				System.load(System.getProperty("user.dir") + "/liblslJNI64.dll");
			}			
		} else {
			if (System.getProperty("os.name").startsWith("Mac")) {
				System.load(System.getProperty("user.dir") + "/liblslJNI.jnilib");
			} else {
				System.load(System.getProperty("user.dir") + "/liblslJNI.so");
			}
		}		
			
        lsl = new liblsl();
        vector_info = lsl.resolve_stream("type","EEG");
        sinlet = new stream_inlet(vector_info.get(0));
        vector = new vectorf();
        
        for (int i=0;i<10;i++){
            sinlet.pull_sample(vector);//get something
            for(int j=0;j<vector.size();j++)
                System.out.println("Got:+"+vector.get(j)+"\n");//print it
        }
    }
}
