using System;
using System.Threading;
using LSL;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            // create a new StreamInfo and declare some meta-data (in accordance with XDF format)
            liblsl.StreamInfo info = new liblsl.StreamInfo("MetaTester","EEG",8,100,liblsl.channel_format_t.cf_float32,"myuid323457");
            liblsl.XMLElement chns = info.desc().append_child("channels");
            String[] labels = {"C3","C4","Cz","FPz","POz","CPz","O1","O2"};
            for (int k=0;k<labels.Length;k++)
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
            liblsl.StreamOutlet outlet = new liblsl.StreamOutlet(info);
        
            // === the following could run on another computer ===
        
            // resolve the stream and open an inlet
            liblsl.StreamInfo[] results = liblsl.resolve_stream("name","MetaTester");
            liblsl.StreamInlet inlet = new liblsl.StreamInlet(results[0]);
            // get the full stream info (including custom meta-data) and dissect it
            liblsl.StreamInfo inf = inlet.info();
            Console.WriteLine("The stream's XML meta-data is: ");
            Console.WriteLine(inf.as_xml());
            Console.WriteLine("The manufacturer is: " + inf.desc().child_value("manufacturer"));
            Console.WriteLine("The cap circumference is: " + inf.desc().child("cap").child_value("size"));
            Console.WriteLine("The channel labels are as follows:");
            liblsl.XMLElement ch = inf.desc().child("channels").child("channel");
            for (int k=0;k<info.channel_count();k++) {
                Console.WriteLine("  " + ch.child_value("label"));
                ch = ch.next_sibling();
            }
            Console.ReadKey();
        }
    }
}
