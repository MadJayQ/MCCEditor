using Blamite.Blam;
using Blamite.Blam.Resources;
using Blamite.Blam.ThirdGen;
using Blamite.Blam.ThirdGen.Structures;
using Blamite.IO;
using Blamite.Serialization;
using Blamite.Serialization.Settings;
using HaloMCCEditor.Core.Logging;
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


        private ResourceTable resourceTable;
        public ResourceTable ResourceTable 
        { 
            get 
            {
                if(resourceTable == null)
                {
                    Logger.AssertMsg(Reader.BaseStream != null, "Cannot read resource table because no valid file stream exists!");
                }
                resourceTable = Get().Resources.LoadResourceTable(Reader);
                return resourceTable;
            } 
        }

        public IReader Reader { get { return fileStream; } }

        public EngineDescription BuildInfo { get { return buildInfo; } }

        private EndianStream fileStream;
        private CacheFileVersionInfo cacheFileVersion;
        private EngineDescription buildInfo;

        //MonkaS
        public ThirdGenHeader ThirdGenCacheFileHeader { get { return (internalCacheFile as ThirdGenCacheFile).FullHeader; } }

        public ICacheFile Get() { return internalCacheFile; }
        private ICacheFile internalCacheFile;


        public long ExpandPointer(uint addr)
        {
            return internalCacheFile.PointerExpander.Expand(addr);
        }

        public long PointerToFileOffset(uint addr)
        {
            long expandedAddr = internalCacheFile.PointerExpander.Expand((uint)addr);
            return internalCacheFile.MetaArea.PointerToOffset(expandedAddr);
        }

        public StructureLayout GetLayout(string layout)
        {
            return BuildInfo.Layouts.GetLayout(layout);
        }

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
