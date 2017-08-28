package examples;
import edu.ucsd.sccn.LSL;

public class HandleMetaData {
    public static void main(String[] args) throws Exception  {
        // create a new StreamInfo and declare some meta-data (in accordance with XDF format)
        LSL.StreamInfo info = new LSL.StreamInfo("MetaTester","EEG",8,100,LSL.ChannelFormat.float32,"myuid323457");
        LSL.XMLElement chns = info.desc().append_child("channels");
        String[] labels = {"C3","C4","Cz","FPz","POz","CPz","O1","O2"};
        for (int k=0;k<labels.length;k++)
            chns.append_child("channel")
                .append_child_value("label", labels[k])
                .append_child_value("unit", "microvolts")
                .append_child_value("type","EEG");
        info.desc().append_child_value("manufacturer","SCCN");
        info.desc().append_child("cap")
            .append_child_value("name","EasyCap")
            .append_child_value("size","54")
            .append_child_value("labelscheme","10-20");

        // create outlet for the stream
        LSL.StreamOutlet outlet = new LSL.StreamOutlet(info);
        
        // === the following could run on another computer ===
        
        // resolve the stream and open an inlet
        LSL.StreamInfo[] results = LSL.resolve_stream("name","MetaTester");
        LSL.StreamInlet inlet = new LSL.StreamInlet(results[0]);
        // get the full stream info (including custom meta-data) and dissect it
        LSL.StreamInfo inf = inlet.info();
        System.out.println("The stream's XML meta-data is: ");
        System.out.println(inf.as_xml());
        System.out.println("The manufacturer is: " + inf.desc().child_value("manufacturer"));
        System.out.println("The cap circumference is: " + inf.desc().child("cap").child_value("size"));
        System.out.println("The channel labels are as follows:");
        LSL.XMLElement ch = inf.desc().child("channels").child("channel");
        for (int k=0;k<info.channel_count();k++) {
            System.out.println("  " + ch.child_value("label"));
            ch = ch.next_sibling();
        }
    }
}