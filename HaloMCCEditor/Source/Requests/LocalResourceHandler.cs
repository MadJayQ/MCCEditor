using System;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Threading.Tasks;
using CefSharp;

using HaloMCCEditor.Core.Logging;

namespace HaloMCCEditor.Core.Requests
{
    public class LocalResourceHandler : ResourceHandler
    {
        public const string SchemeName = "mcc";

        private DirectoryInfo sourceDirectoryInfo;
        private string frontendFolderPath;
        public LocalResourceHandler()
        {
            //MonkaS need a precompiler directive to check for whether or not we're in VS since prod path will be different.
            frontendFolderPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory + "../../../../EditorBundle");
            sourceDirectoryInfo = new DirectoryInfo(frontendFolderPath);

            Debug.Assert(sourceDirectoryInfo != null, "No editor source directory supplied!");
        }

        public static string FormatURL(string url)
        {
            return string.Format("{0}://" + url, SchemeName);
        }

        public override CefReturnValue ProcessRequestAsync(IRequest request, ICallback callback)
        {
            Uri uri = new Uri(request.Url);
            string fileName = uri.Authority;
            if(!uri.AbsolutePath.Equals("/"))
            {
                fileName += uri.AbsolutePath;
            }

            string requestedFilePath = frontendFolderPath + '/' + fileName;

            DirectoryInfo filePathDirectoryInfo = new DirectoryInfo(requestedFilePath);

            bool isAccessToFilePermitted = IsRequestedPathValid(filePathDirectoryInfo, sourceDirectoryInfo);

            if(filePathDirectoryInfo.Attributes == FileAttributes.Normal)
            {
                Logger.LogReport("Yo wtf");
            }

            if (isAccessToFilePermitted && File.Exists(requestedFilePath))
            {

                Task.Run(() =>
                {
                    byte[] bytes = File.ReadAllBytes(requestedFilePath);
                    Stream = new MemoryStream(bytes);

                    var fileExtension = Path.GetExtension(fileName);
                    MimeType = Cef.GetMimeType(fileExtension);

                    callback.Continue();
                    return CefReturnValue.Continue;
                });

                return CefReturnValue.ContinueAsync;
            }

            StatusCode = (int)HttpStatusCode.NotFound;
            callback.Dispose();
            return CefReturnValue.Cancel;
        }

        public bool IsRequestedPathValid(DirectoryInfo path, DirectoryInfo folder)
        {
            if (path.Parent == null)
            {
                return false;
            }

            if (string.Equals(path.Parent.FullName, folder.FullName, StringComparison.OrdinalIgnoreCase))
            {
                return true;
            }

            return IsRequestedPathValid(path.Parent, folder);
        }
    }
}
