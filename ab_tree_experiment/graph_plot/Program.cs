using System;
using System.Collections.Generic;
using System.IO;

namespace GraphPlot
{
    /*
     *  Creates files with Python code for graph plotting.
     *  
     *  Input files:  
     *  
     *      name:       [test_name]_2_3.txt
     *                  [test_name]_2_4.txt
     *                  
     *      lines:      'x y'
     *                  
     *                  
     *  Output files:   
     *  
     *      name:       [test_name]_plot.py
     *      
     *      graph:      (2,3)-tree  -black curve 
     *                  (2,4)-tree  -green curve
     */
    class Program
    {
        static void Main(string[] args)
        {
            foreach (string name in args)
                rewriteToPlotCode(name);
            Console.WriteLine("Done.");
        }

        static void rewriteToPlotCode(string name)
        {
            StreamWriter w = new StreamWriter(name + "_plot.py");
            w.WriteLine(code_start);
         
            StreamReader r1 = new StreamReader(name+ "_2_3"+".txt");
            readAndWriteValues(r1, w);
            w.WriteLine(axes_end.Replace("color_value", "black"));
            r1.Close();

            StreamReader r2 = new StreamReader(name + "_2_4" + ".txt");
            readAndWriteValues(r2, w);
            w.WriteLine(
                axes_end.Replace("color_value", "green") 
                + "fig.savefig('"
                + name.Substring(name.Length-10) +"_graph.png')");  //!!
            
            w.Close();
        }

        static void readAndWriteValues(StreamReader r, StreamWriter w)
        {
            string line;
            string[] words;
            List<string> x = new List<string>();
            List<string> y = new List<string>();
            while (!r.EndOfStream)
            {
                line = r.ReadLine();
                words = line.Split();
                x.Add(words[0]);
                y.Add(words[1]);
            }
            string x_values = "[" + String.Join(", ", x) + "] ";
            string y_values = "[" + String.Join(", ", y) + "] ";
            w.WriteLine("ax.plot(");
            w.WriteLine(x_values + ",");
            w.WriteLine(y_values);
        }



        static string code_start =
@"import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt

fig = plt.figure(figsize = (10, 8))
ax = fig.add_subplot(111)

plt.title('The random test')
plt.xlabel('The set size')
plt.ylabel('The average number of structural changes')
plt.ylim(0, 1)

";

        static string axes_end =
@", '-ok', markersize=3
,color='color_value'
)

"
;

    }
}
