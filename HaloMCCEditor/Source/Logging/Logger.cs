using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace HaloMCCEditor.Core.Logging
{
    public class Logger
    {
        //Singleton design pattern
        private static Logger instance;

        public static Logger Instance
        {
            get
            {
                if(instance == null)
                {
                    instance = new Logger();
                }
                return instance;
            }
        }

        private Logger() { } //Hide constructor from outside classes

        private void ImplLogReport(string format, params object[] param)
        {
            Console.Write(string.Format(format, param));
        }

        public static void LogReport(string format, params object[] param)
        {
            instance.ImplLogReport(format, param);
        }
    }
}
