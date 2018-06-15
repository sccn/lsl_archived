using System;
using System.Threading;
using LSL;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            // wait until an EEG stream shows up
            liblsl.StreamInfo[] results = liblsl.resolve_stream("type", "EEG");

            // open an inlet and print meta-data
            liblsl.StreamInlet inlet = new liblsl.StreamInlet(results[0]);
            System.Console.Write(inlet.info().as_xml());

            // read samples
            float[,] buffer = new float[512, 8];
            double[] timestamps = new double[512];
            while (true)
            {
                int num = inlet.pull_chunk(buffer,timestamps);
                for (int s = 0; s < num; s++)
                {
                    for (int c = 0; c < 8; c++)
                        Console.Write("\t{0}", buffer[s, c]);
                    Console.WriteLine();
                }
            }
        }
    }
}
