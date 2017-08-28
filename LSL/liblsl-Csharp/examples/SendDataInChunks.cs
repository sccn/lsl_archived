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

			// send data in chunks of 10 samples and 8 channels
            float[,] data = new float[10, 8];
            while (true)
            {
                for (int s = 0; s < 10; s++)
                    for (int c = 0; c < 8; c++)
                        data[s, c] = rnd.Next(-100, 100);
                outlet.push_chunk(data);
                System.Threading.Thread.Sleep(100);
            }
        }
    }
}
