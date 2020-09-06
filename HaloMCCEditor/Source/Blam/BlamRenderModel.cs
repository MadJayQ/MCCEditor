using Blamite.Blam;
using Blamite.IO;
using Blamite.Serialization;
using Blamite.Blam.Resources.Models;

using HaloMCCEditor.Core.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HaloMCCEditor.Core.Blam
{
    public class BlamRenderModel
    {
        public class BlamRenderModelPermutation
        {
            public string Name { get; private set; }
            public Int16 ModelSection { get; private set; }
            public Int16 ModelCount { get; private set; }

            public BlamRenderModelPermutation(long addr, BlamCacheFile blamCacheFile, StructureLayout permutationLayout)
            {
                blamCacheFile.Reader.SeekTo(addr);
                StructureValueCollection values = StructureReader.ReadStructure(blamCacheFile.Reader, permutationLayout);

                Name = blamCacheFile.Get().StringIDs.GetString(new StringID(values.GetInteger("name stringid")));
                ModelSection = (Int16)(values.GetInteger("model section"));
                ModelCount = (Int16)(values.GetInteger("model count"));

            }

        }
        public class BlamRenderModelRegion
        {
            public string Name { get; private set; }
            public List<BlamRenderModelPermutation> Permutations { get; private set; }
            public BlamRenderModelRegion(long addr, BlamCacheFile blamCacheFile)
            {
                Permutations = new List<BlamRenderModelPermutation>();

                StructureLayout regionLayout = blamCacheFile.GetLayout("model region");
                StructureLayout permutationLayout = blamCacheFile.GetLayout("model permutation");

                blamCacheFile.Reader.SeekTo(addr);
                StructureValueCollection values = StructureReader.ReadStructure(blamCacheFile.Reader, regionLayout);

                Name = blamCacheFile.Get().StringIDs.GetString(new StringID(values.GetInteger("name stringid")));
                ulong numPermutations = values.GetInteger("number of permutations");
                long permutationTableAddress = blamCacheFile.ExpandPointer((uint)values.GetInteger("permutation table address"));
                long permutationRegionTableOffset = blamCacheFile.Get().MetaArea.PointerToOffset(permutationTableAddress);
                for (ulong permutationNum = 0; permutationNum < numPermutations; permutationNum++)
                {
                    long permutationAddr = permutationRegionTableOffset + (long)((ulong)permutationLayout.Size * permutationNum);
                    Permutations.Add(new BlamRenderModelPermutation(permutationAddr, blamCacheFile, permutationLayout));
                }

            }
        }
        public List<BlamRenderModelRegion> Regions { get; private set; }
        public BlamModelRenderGeometry Geometry { get; private set; }

        private StructureValueCollection renderModelValues;

        public BlamRenderModel(ITag modelTag, BlamCacheFile cacheFile)
        {
            LoadRegions(modelTag, cacheFile);
            LoadRenderGeometry(modelTag, cacheFile);
        }

        private void LoadRegions(ITag modelTag, BlamCacheFile blamCacheFile)
        {
            IReader reader = blamCacheFile.Reader;
            StructureLayout renderModelLayout = blamCacheFile.GetLayout("render model");

            reader.SeekTo(modelTag.MetaLocation.AsOffset());
            renderModelValues = StructureReader.ReadStructure(blamCacheFile.Reader, renderModelLayout);

            ulong numRegions = renderModelValues.GetInteger("number of regions");
            ulong regionTableAddress = renderModelValues.GetInteger("region table address");

            long expandedRegionTableOffset = blamCacheFile.PointerToFileOffset((uint)regionTableAddress);

            StructureLayout modelRegionLayout = blamCacheFile.GetLayout("model region");

            Regions = new List<BlamRenderModelRegion>();

            for(ulong regionNum = 0ul; regionNum < numRegions; regionNum++)
            {
                long regionPointer = expandedRegionTableOffset + (long)((ulong)modelRegionLayout.Size * regionNum);
                Regions.Add(new BlamRenderModelRegion(regionPointer, blamCacheFile));
            }
        }

        private void LoadRenderGeometry(ITag modelTag, BlamCacheFile blamCacheFile)
        {
            ulong runtimeGeometryFlags = renderModelValues.GetInteger("render geometry runtime flags");
            ulong numMeshes = renderModelValues.GetInteger("number of sections");
            ulong meshTableAddress = renderModelValues.GetInteger("section table address");

            ulong meshTableOffset = (ulong)blamCacheFile.PointerToFileOffset((uint)meshTableAddress);

            Geometry = new BlamModelRenderGeometry((RenderGeometryRuntimeFlags)runtimeGeometryFlags, meshTableOffset, numMeshes);
            Geometry.Read(blamCacheFile);
        }
    }
}
