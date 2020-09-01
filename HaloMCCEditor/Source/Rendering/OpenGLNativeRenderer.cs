using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace HaloMCCEditor.Core.Rendering
{
    public class OpenGLNativeRenderer : INativePlatformRenderer
    {
        public void CleanupTargetResources()
        {
            throw new NotImplementedException();
        }

        public void InitializePlatformRenderer(IntPtr windowHandle, Rect targetRect)
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

        public void ResizeTargetResources(Rect newWindowRect)
        {
            throw new NotImplementedException();
        }
    }
}
