using Blamite.Blam;
using Blamite.Blam.ThirdGen;
using Blamite.IO;
using Blamite.Serialization;
using Blamite.Serialization.Settings;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HaloMCCEditor.Core.Blam
{
    public class BlamCacheFile : IDisposable
    {

        private static ICacheFile CreateThirdGenCacheFile(IReader inputReader, EngineDescription buildInfo, CacheFileVersionInfo cacheFileVersion)
        {
            string buildString = cacheFileVersion.BuildString;

            return new ThirdGenCacheFile(inputReader, buildInfo, buildString);
        }

        public IReader Reader { get { return fileStream; } }

        public EngineDescription BuildInfo { get { return buildInfo; } }

        private EndianStream fileStream;
        private CacheFileVersionInfo cacheFileVersion;
        private EngineDescription buildInfo;

        public ICacheFile Get() { return internalCacheFile; }
        private ICacheFile internalCacheFile;

        public BlamCacheFile(string filePath)
        {
            if(!File.Exists(filePath))
            {
                throw new FileNotFoundException("Invalid cache file path");
            }

            fileStream = new EndianStream(File.Open(filePath, FileMode.Open, FileAccess.ReadWrite), Endian.LittleEndian);
            cacheFileVersion = new CacheFileVersionInfo(fileStream);
            EngineDatabase database = XMLEngineDatabaseLoader.LoadDatabase("Formats/Engines.xml");
            buildInfo = database.FindEngineByVersion(cacheFileVersion.BuildString);

            switch (cacheFileVersion.Engine)
            {
                case EngineType.ThirdGeneration:
                    internalCacheFile = CreateThirdGenCacheFile(fileStream, buildInfo, cacheFileVersion);
                    break;
                default:
                    throw new InvalidOperationException("Only third generation engine map files are supported at the moment!");
            }

        }

        public void Dispose()
        {
            fileStream.Dispose();
        }
    }
}
