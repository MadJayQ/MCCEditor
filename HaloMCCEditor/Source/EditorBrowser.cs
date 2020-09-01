using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using CefSharp;
using CefSharp.WinForms;

namespace HaloMCCEditor.Core
{
    class EditorBrowser : ChromiumWebBrowser
    {
        public EditorBrowser(string initialURL) : base(initialURL)
        {

        }
    }
}
