using HaloMCCEditor.Core.Rendering;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Windows.Forms;

namespace HaloMCCEditor.Core
{
    public class ViewportHostControl : UserControl 
    {

        private INativePlatformRenderer nativeRenderer;

        public ViewportHostControl()
        {
            Load += new System.EventHandler(InitializeVulkan);
            SizeChanged += new System.EventHandler(OnControlResize);

            nativeRenderer = new VulkanNativeRenderer(); 
        }

        public virtual void OnControlResize(object sender, EventArgs e)
        {

        }

        public virtual void InitializeVulkan(object sender, EventArgs e)
        {

        }

        protected override void OnPaintBackground(PaintEventArgs e)
        {
            
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
        }
    }
}
