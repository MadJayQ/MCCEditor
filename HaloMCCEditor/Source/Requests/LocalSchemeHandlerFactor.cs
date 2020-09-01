using System;
using CefSharp;

namespace HaloMCCEditor.Core.Requests
{
    public class LocalSchemeHandlerFactory : ISchemeHandlerFactory
    {
        public IResourceHandler Create(IBrowser browser, IFrame frame, string schemeName, IRequest request)
        {
            return new LocalResourceHandler();
        }
    }
}
