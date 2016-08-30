using System;
using System.Threading;
using LSL;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            Random rnd = new Random();

            // create stream info and outlet
            liblsl.StreamInfo info = new liblsl.StreamInfo("BioSemi", "EEG", 8, 100, liblsl.channel_format_t.cf_float32, "sddsfsdf");
            liblsl.StreamOutlet outlet = new liblsl.StreamOutlet(info);
            float[] data = new float[8];
            while (true)
            {
                // generate random data and send it
                for (int k = 0; k < data.Length; k++)
                    data[k] = rnd.Next(-100, 100);
                outlet.push_sample(data);
                System.Threading.Thread.Sleep(10);
            }

            System.Console.ReadKey();
        }
    }
}
