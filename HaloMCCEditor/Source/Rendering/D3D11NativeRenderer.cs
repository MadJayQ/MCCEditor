using CefSharp.Structs;
using CefSharp.Wpf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace HaloMCCEditor.Core.Rendering
{
    public class D3D11NativeRenderer : INativePlatformRenderer
    {
        public void CleanupTargetResources()
        {
            throw new NotImplementedException();
        }

        public void InitializePlatformRenderer(IntPtr windowHandle, System.Windows.Rect targetRect)
        {
            throw new NotImplementedException();
        }

        public void RenderBeginFrame()
        {
            throw new NotImplementedException();
        }

        public void RenderEndFrame()
        {
            throw new NotImplementedException();
        }

        public void ResizeTargetResources(System.Windows.Rect newWindowRect)
        {
            throw new NotImplementedException();
        }
    }
}
