using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HaloMCCEditor.Core.Input
{


    public class Win32NativeMessageHandler
    {
        public const int WM_SYSKEYDOWN = 0x104;
        public const int WM_KEYDOWN = 0x100;
        public const int WM_KEYUP = 0x101;
        public const int WM_SYSKEYUP = 0x105;
        public const int WM_CHAR = 0x102;
        public const int WM_SYSCHAR = 0x106;
        public const int VK_TAB = 0x9;
        public const int VK_LEFT = 0x25;
        public const int VK_UP = 0x26;
        public const int VK_RIGHT = 0x27;
        public const int VK_DOWN = 0x28;

        public bool ShouldProcessKey(int windowsKeyCode)
        {
            return (windowsKeyCode != VK_TAB && 
                windowsKeyCode != VK_LEFT && 
                windowsKeyCode != VK_UP && 
                windowsKeyCode != VK_DOWN && 
                windowsKeyCode != VK_RIGHT);
        }

    }
}
