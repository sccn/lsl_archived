using System;
using System.Threading;
using LSL;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            // create stream info and outlet
            liblsl.StreamInfo inf = new liblsl.StreamInfo("Test1", "Markers", 1, 0, liblsl.channel_format_t.cf_string, "giu4569");
            liblsl.StreamOutlet outl = new liblsl.StreamOutlet(inf);

            Random rnd = new Random();
            string[] strings = new string[] { "Test", "ABC", "123" };
            string[] sample = new string[1];
            for (int k = 0; ; k++)
            {
                // send a marker and wait for a random interval
                sample[0] = strings[k % 3];
                outl.push_sample(sample);
                System.Threading.Thread.Sleep(rnd.Next(1000));
            }
        }
    }
}
