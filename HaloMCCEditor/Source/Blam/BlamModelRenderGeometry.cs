using Blamite.Blam;
using Blamite.Blam.Resources;
using Blamite.IO;
using Blamite.Serialization;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Animation;

using HaloMCCEditor.Core.Logging;

namespace HaloMCCEditor.Core.Blam
{
    [Flags]
    public enum RenderGeometryRuntimeFlags : int
    {
        None = 0,
        Processed = 1 << 0,
        Available = 1 << 1,
        HasValidBudgets = 1 << 2,
        ManualResourceCalibration = 1 << 3,
        KeepRawGeometry = 1 << 4,
        DoNotUseCompressedVertexPositions = 1 << 5,
        PcaAnimationTableSorted = 1 << 6,
        HasDualQuat = 1 << 7
    }

    public class BlamModelMesh
    {
        [Flags]
        public enum MeshVertexType : byte
        {
            World,
            Rigid,
            Skinned,
            ParticleModel,
            FlatWorld,
            FlatRigid,
            FlatSkinned,
            Screen,
            Debug,
            Transparent,
            Particle,
            Contrail,
            LightVolume,
            SimpleChud,
            FancyChud,
            Decorator,
            TinyPosition,
            PatchyFog,
            Water,
            Ripple,
            Implicit,
            Beam,
            DualQuat
        }


        public class BlamModelSubmesh
        {
            public int SubmeshIdx { get; private set; }
            public Int16 ShaderIdx { get; private set; }
            public Int16 IndexBufferStart { get; private set; }
            public Int16 IndexBufferCount { get; private set; }
            public Int16 VertexGroupStart { get; private set; }
            public Int16 VertexGroupCount { get; private set; }
            public Int16 VertexBufferCount { get; private set; }

            public BlamModelSubmesh(uint baseTableOffset, int submeshIdx, BlamCacheFile cacheFile)
            {
                IReader reader = cacheFile.Reader;
                SubmeshIdx = submeshIdx;

                StructureLayout submeshLayout = cacheFile.GetLayout("model submesh");
                reader.SeekTo(baseTableOffset + (submeshIdx * submeshLayout.Size));
                StructureValueCollection submeshValues = StructureReader.ReadStructure(reader, submeshLayout);

                ShaderIdx = (Int16)submeshValues.GetInteger("shader index");
                IndexBufferStart = (Int16)submeshValues.GetInteger("index buffer start");
                IndexBufferCount = (Int16)submeshValues.GetInteger("index buffer count");
                VertexGroupStart = (Int16)submeshValues.GetInteger("subpart index");
                VertexGroupCount = (Int16)submeshValues.GetInteger("subpart count");
                VertexBufferCount = (Int16)submeshValues.GetInteger("vertex buffer count");
            }
        }
        public class BlamModelVertexGroup
        {
            public int VertexGroupIndex { get; private set; }
            public Int32 IndexBufferStart { get; private set; }
            public Int32 IndexBufferCount { get; private set; }
            public Int16 ParentSubmeshIndex { get; private set; }
            public Int16 VertexBufferCount { get; private set; }
            public BlamModelVertexGroup(uint baseTableOffset, int vertexGroupIdx, BlamCacheFile cacheFile)
            {
                IReader reader = cacheFile.Reader;
                VertexGroupIndex = vertexGroupIdx;

                StructureLayout vertexGroupLayout = cacheFile.GetLayout("model vertex group");
                reader.SeekTo(baseTableOffset + (vertexGroupIdx * vertexGroupLayout.Size));
                StructureValueCollection vertexGroupValues = StructureReader.ReadStructure(reader, vertexGroupLayout);

                IndexBufferStart = (Int32)vertexGroupValues.GetInteger("index buffer start");
                IndexBufferCount = (Int32)vertexGroupValues.GetInteger("index buffer count");
                ParentSubmeshIndex = (Int16)vertexGroupValues.GetInteger("parent submesh index");
                VertexBufferCount = (Int16)vertexGroupValues.GetInteger("vertex buffer count");
            }
        }
        public List<BlamModelSubmesh> Submeshes { get; private set; }
        public List<BlamModelVertexGroup> VertexGroups { get; private set; }
        public int MeshIndex { get; private set; }
        public MeshVertexType VertexType { get; private set; }

        public Int16[] VertexBufferIndices = new Int16[8];
        public Int16[] IndexBufferIndices = new Int16[2];

        public BlamModelMesh(BlamCacheFile cacheFile, int index, StructureLayout meshLayout)
        {
            MeshIndex = index;
            LoadMeshData(cacheFile, meshLayout);
        }

        private void LoadMeshData(BlamCacheFile cacheFile, StructureLayout meshLayout)
        {
            IReader reader = cacheFile.Reader;
            StructureValueCollection meshValueCollection = StructureReader.ReadStructure(reader, meshLayout);

            ulong numSubmeshes = meshValueCollection.GetInteger("number of submeshes");
            ulong numVertexGroups = meshValueCollection.GetInteger("number of vertex groups");

            uint submeshTableOffset = (uint)cacheFile.PointerToFileOffset((uint)meshValueCollection.GetInteger("submesh table address"));
            uint vertexGroupTableOffset = (uint)cacheFile.PointerToFileOffset((uint)meshValueCollection.GetInteger("vertex group table address"));

            Submeshes = new List<BlamModelSubmesh>((int)numSubmeshes);
            for(int i = 0; i < Submeshes.Capacity; i++)
            {
                Submeshes.Add(new BlamModelSubmesh(submeshTableOffset, i, cacheFile));
            }
            VertexGroups = new List<BlamModelVertexGroup>((int)numVertexGroups);
            for(int i = 0; i < VertexGroups.Capacity; i++)
            {
                VertexGroups.Add(new BlamModelVertexGroup(vertexGroupTableOffset, i, cacheFile));
            }
            VertexType = (MeshVertexType)meshValueCollection.GetInteger("vertex format");

            for(int i = 1; i <= VertexBufferIndices.Length; i++)
            {
                string indexerString = string.Format("vertex buffer {0}", i);
                VertexBufferIndices[i - 1] = (Int16)meshValueCollection.GetInteger(indexerString);
            }

            for(int i = 1; i <= IndexBufferIndices.Length; i++)
            {
                string indexerString = string.Format("index buffer {0}", i);
                IndexBufferIndices[i - 1] = (Int16)meshValueCollection.GetInteger(indexerString);
            }
        }
    }


    public class BlamModelRenderGeometry
    {

        public RenderGeometryRuntimeFlags RuntimeFlags { get; private set; }
        public List<BlamModelMesh> Meshes { get; private set; }

        private long meshTableOffset;

        public BlamModelRenderGeometry(RenderGeometryRuntimeFlags flags, ulong tableAddr, ulong numMeshes)
        {
            Meshes = new List<BlamModelMesh>((int)numMeshes);
            RuntimeFlags = flags;
            meshTableOffset = (long)tableAddr;
        }

        private byte[] ReadResourcePageData(BlamCacheFile cacheFile, ResourcePage page)
        {
            BlamCacheFile cache = cacheFile;
            string cacheFilePath = "";
            bool cleanup = false;
            if(page.FilePath != null)
            {
                cacheFilePath = Util.VisualStudioProvider.TryGetSolutionDirectoryInfo().FullName + "\\" + page.FilePath;
                cleanup = true;
                cache = new BlamCacheFile(cacheFilePath);
            }

            byte[] decompressed = new byte[page.UncompressedSize];
            cacheFile.Reader.SeekTo(page.Offset);

            byte[] compressed = cacheFile.Reader.ReadBlock(page.CompressedSize);

            if(page.CompressionMethod == ResourcePageCompression.None)
            {
                decompressed = compressed;
            }
            else if(page.CompressionMethod == ResourcePageCompression.Deflate)
            {
                using (DeflateStream stream = new DeflateStream(new MemoryStream(compressed), CompressionMode.Decompress))
                    stream.Read(decompressed, 0, BitConverter.ToInt32(BitConverter.GetBytes(page.UncompressedSize), 0));
            }


            if(cleanup)
            {
                cache.Dispose();
            }

            return decompressed;
        }

        private void ReadResourceBuffers(BlamCacheFile cacheFile, ref Resource resourceRef)
        {
            bool[] convertedVertexBuffers = new bool[100];
            bool[] convertedIndexBuffers = new bool[100];

            using(EndianWriter writer = new EndianWriter(new MemoryStream(resourceRef.Info), Endian.LittleEndian))
            {
                foreach (ResourceFixup fixup in resourceRef.ResourceFixups)
                {
                    BlamCacheAddress address = new BlamCacheAddress(fixup.Address);
                    writer.SeekTo(fixup.Offset);
                    writer.WriteUInt32(address.Value);
                }
            }

            byte[] primaryResource = ReadResourcePageData(cacheFile, resourceRef.Location.PrimaryPage);
            byte[] secondaryResource = ReadResourcePageData(cacheFile, resourceRef.Location.SecondaryPage);

            using(EndianReader definitionReader = new EndianReader(new MemoryStream(resourceRef.Info), Endian.LittleEndian))
            using(EndianReader primaryReader = new EndianReader(new MemoryStream(primaryResource), Endian.LittleEndian))
            using(EndianReader secondaryReader = new EndianReader(new MemoryStream(secondaryResource), Endian.LittleEndian))
            {
                BlamCacheAddress cacheAddress = new BlamCacheAddress((uint)resourceRef.BaseDefinitionAddress);
                Logger.AssertMsg(cacheAddress.Type == BlamCacheAddressType.Definition, "INVALID CACHE ADDRESS");
                definitionReader.SeekTo(cacheAddress.Offset);
                StructureLayout layout = cacheFile.GetLayout("render geometry api resource definition");
                StructureValueCollection values = StructureReader.ReadStructure(definitionReader, layout);

                BlamCacheAddress uselessCrap3Address = new BlamCacheAddress((uint)values.GetInteger("address of useless crap3"));
                BlamCacheAddress uselessCrap4Address = new BlamCacheAddress((uint)values.GetInteger("address of useless crap4"));
            }
        }

        public void Read(BlamCacheFile cacheFile, Resource resourceRef)
        {
            IReader reader = cacheFile.Reader;
            StructureLayout meshLayout = cacheFile.GetLayout("model section");

            for(int i = 0; i < Meshes.Capacity; i++)
            {
                reader.SeekTo(meshTableOffset + (i * meshLayout.Size));
                Meshes.Add(new BlamModelMesh(cacheFile, i, meshLayout));
            }

            ReadResourceBuffers(cacheFile, ref resourceRef);
        }
    }
}
