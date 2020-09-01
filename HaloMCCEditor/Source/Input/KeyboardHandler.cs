using System;

using CefSharp;

namespace HaloMCCEditor.Core.Input
{
    public class KeyboardHandler : IKeyboardHandler
    {
        private Win32NativeMessageHandler nativeMessageHandler = new Win32NativeMessageHandler();
        public bool OnKeyEvent(IWebBrowser chromiumWebBrowser, IBrowser browser, KeyType type, int windowsKeyCode, int nativeKeyCode, CefEventFlags modifiers, bool isSystemKey)
        {
            return false;
        }

        public bool OnPreKeyEvent(IWebBrowser chromiumWebBrowser, IBrowser browser, KeyType type, int windowsKeyCode, int nativeKeyCode, CefEventFlags modifiers, bool isSystemKey, ref bool isKeyboardShortcut)
        {
            if (!nativeMessageHandler.ShouldProcessKey(windowsKeyCode))
                return false;
            return false;
        }
    }
}
