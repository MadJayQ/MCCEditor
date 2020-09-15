using Blamite.Blam;
using Blamite.Blam.Resources;
using HaloMCCEditor.Core.Blam;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HaloMCCEditor.Core.Blam.Resources
{
    public class BlamTagResource
    {
        protected byte[] ReadResourcePageData(BlamCacheFile cacheFile, ResourcePage page)
        {
            BlamCacheFile cache = cacheFile;
            string cacheFilePath = "";
            bool cleanup = false;
            if (page.FilePath != null)
            {
                cacheFilePath = Core.Util.VisualStudioProvider.TryGetSolutionDirectoryInfo().FullName + "\\" + page.FilePath;
                cleanup = true;
                cache = new BlamCacheFile(cacheFilePath);
            }

            byte[] decompressed = new byte[page.UncompressedSize];
            cacheFile.Reader.SeekTo(page.Offset);

            byte[] compressed = cacheFile.Reader.ReadBlock(page.CompressedSize);

            if (page.CompressionMethod == ResourcePageCompression.None)
            {
                decompressed = compressed;
            }
            else if (page.CompressionMethod == ResourcePageCompression.Deflate)
            {
                using (DeflateStream stream = new DeflateStream(new MemoryStream(compressed), CompressionMode.Decompress))
                    stream.Read(decompressed, 0, BitConverter.ToInt32(BitConverter.GetBytes(page.UncompressedSize), 0));
            }


            if (cleanup)
            {
                cache.Dispose();
            }

            return decompressed;
        }
        protected virtual void LoadResourceData(BlamCacheFile cacheFile, ref Resource resourceRef) { }
    }
}
