using System;
using System.Diagnostics;
using System.Windows;
using CefSharp;
using CefSharp.WinForms;
using HaloMCCEditor.Core;
using HaloMCCEditor.Core.Requests;
using HaloMCCEditor.Source.UnitTests;

namespace HaloMCCEditor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 
    
    public partial class MainWindow : Window
    {
        public ChromiumWebBrowser MainView = null;
        private void InstallWindowsHandlers()
        {
            Debug.Assert(MainView != null, "NO MAIN VIEW CREATED");

            MainView.KeyboardHandler = new Core.Input.KeyboardHandler();
            MainView.MenuHandler = new Core.Input.ContextMenuHandler();
        }

        private void InitializeChromium()
        {
            CefSettings settings = new CefSettings();
            settings.RegisterScheme(new CefCustomScheme()
            {
                SchemeName = LocalResourceHandler.SchemeName,
                SchemeHandlerFactory = new LocalSchemeHandlerFactory()
            });

            //Enable hardware rendering
            settings.WindowlessRenderingEnabled = true;

            settings.CefCommandLineArgs.Add("enable-gpu", "1");
            settings.CefCommandLineArgs.Add("enable-webgl", "1");
            settings.CefCommandLineArgs.Add("enable-begin-frame-scheduling", "1");
            settings.CefCommandLineArgs.Add("--off-screen-frame-rate", "60");
            //Enable remote debugging
            settings.RemoteDebuggingPort = 8088;

            if(!Cef.IsInitialized)
            {
                Cef.Initialize(settings);
            }

            BrowserSettings browserSettings = new BrowserSettings();
            browserSettings.WindowlessFrameRate = 60;

            MainView = new EditorBrowser("https://anachron.games/chroma-scape/");
            MainView.BrowserSettings = browserSettings;
        }

        public MainWindow()
        {

            InitializeComponent();

            InitializeChromium();
            InstallWindowsHandlers();

#if DEBUG
            EditorUnitTests.RunUnitTests();
#endif
        }
    }
}
