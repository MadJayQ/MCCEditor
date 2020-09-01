using System;
using System.Windows;

namespace HaloMCCEditor.Core.Rendering
{
    public class VulkanNativeRenderer : INativePlatformRenderer
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
