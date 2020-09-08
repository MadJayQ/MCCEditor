using Blamite.Blam.ThirdGen;
using HaloMCCEditor.Core.Logging;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HaloMCCEditor.Core.Blam;
using Blamite.Blam;
using Blamite.Util;
using Blamite.Serialization;
using Blamite.Blam.Util;
using Blamite.Blam.ThirdGen.Structures;
using Blamite.IO;

namespace HaloMCCEditor.Source.UnitTests
{
    public static class EditorUnitTests
    {
        //private const string TARGET_MAP_FILE = "F:\\Games\\Steam\\steamapps\\common\\Halo The Master Chief Collection\\halo3\\maps\\warehouse.map";
        private const string TARGET_MAP_FILE = "C:\\Users\\NASCARAdmin\\Documents\\GitHub\\MCCEditor\\Maps\\warehouse.map";
        private const string TARGET_MODEL = "objects\\vehicles\\warthog\\warthog";
        private static void MapLoadUnitTest()
        {
            using (BlamCacheFile blamCacheFile = new BlamCacheFile(TARGET_MAP_FILE))
            {
                const int VEHICLE_MODEL_OFFSET = 0x34;
                ICacheFile cacheFile = blamCacheFile.Get();
                IEnumerable<ITag> vehicleTags = cacheFile.Tags.FindTagsByGroup("vehi");
                StructureLayout layout = blamCacheFile.BuildInfo.Layouts.GetLayout("tag reference");
                foreach (ITag vehicleTag in vehicleTags)
                {
                    string name = cacheFile.FileNames.GetTagName(vehicleTag);

                    //This is where in the memory region that the map exists a structure for the meta data exists
                    long tagRefPointer = vehicleTag.MetaLocation.AsOffset() + VEHICLE_MODEL_OFFSET;
                    blamCacheFile.Reader.SeekTo(tagRefPointer);
                    StructureValueCollection collection = StructureReader.ReadStructure(blamCacheFile.Reader, layout); //Try to read it

                    //Extract fields
                    ulong groupMagic = collection.GetInteger("tag group magic");
                    DatumIndex datumIndex = new DatumIndex(collection.GetInteger("datum index"));
                    //Is this a valid datum?
                    bool isValid = cacheFile.Tags.IsValidIndex(datumIndex);

                    ITag vehicleModelTag = cacheFile.Tags[datumIndex];
                    string vehicleModelName = cacheFile.FileNames.GetTagName(vehicleModelTag);
                    string group = CharConstant.ToString(vehicleModelTag.Group.Magic);
                    if (!TARGET_MODEL.Equals(vehicleModelName))
                    {
                        continue;
                    }

                    tagRefPointer = vehicleModelTag.MetaLocation.AsOffset();
                    blamCacheFile.Reader.SeekTo(tagRefPointer);
                    collection = StructureReader.ReadStructure(blamCacheFile.Reader, layout);
                    groupMagic = collection.GetInteger("tag group magic");
                    datumIndex = new DatumIndex(collection.GetInteger("datum index"));

                    isValid = cacheFile.Tags.IsValidIndex(datumIndex);

                    ITag vehicleRenderModelTag = cacheFile.Tags[datumIndex];
                    string renderModelTagName = CharConstant.ToString(vehicleRenderModelTag.Group.Magic);

                    BlamRenderModel warthogModel = new BlamRenderModel(vehicleRenderModelTag, blamCacheFile);
                }
            }
            Logger.LogReport("MapLoadUnitTest: Success!");
        }

        public static void RunUnitTests()
        {
            try
            {
                MapLoadUnitTest();
                Logger.LogReport("All unit tests executed successfully!");
            }
            catch (Exception ex)
            {
                Logger.AssertMsgFormat(false, "FAIL: {0}", ex.Message);
            }
        }

    }
}
