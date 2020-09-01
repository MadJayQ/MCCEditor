using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace HaloMCCEditor.Core.Rendering
{
    interface INativePlatformRenderer
    {
        void InitializePlatformRenderer(IntPtr windowHandle, Rect targetRect);
        void ResizeTargetResources(Rect newWindowRect);
        void CleanupTargetResources();

        void RenderBeginFrame();
        void RenderEndFrame();
    }
}
