using Blamite.Blam.Resources;
using Blamite.IO;
using Blamite.Serialization;
using HaloMCCEditor.Core.Logging;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HaloMCCEditor.Core.Blam.Resources
{
    public class BlamRenderGeometryApiResourceDefinition : BlamTagResource
    {
        public BlamD3DPointer<VertexBufferDefinition>[] VertexBufferDefinitions;
        public BlamD3DPointer<IndexBufferDefinition>[] IndexBufferDefinitions;

        protected override void LoadResourceData(BlamCacheFile cacheFile, ref Resource resourceRef)
        {
            bool[] convertedVertexBuffers = new bool[100];
            bool[] convertedIndexBuffers = new bool[100];

            using (EndianWriter writer = new EndianWriter(new MemoryStream(resourceRef.Info), Endian.LittleEndian))
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

            //The using keyword will automatically call IDisposable.Dispose at the end of scope
            //Automatically freeing up our resources and closing our file streams

            using (EndianReader definitionReader = new EndianReader(new MemoryStream(resourceRef.Info), Endian.LittleEndian))
            using (EndianReader primaryReader = new EndianReader(new MemoryStream(primaryResource), Endian.LittleEndian))
            using (EndianReader secondaryReader = new EndianReader(new MemoryStream(secondaryResource), Endian.LittleEndian))
            {
                BlamCacheAddress cacheAddress = new BlamCacheAddress((uint)resourceRef.BaseDefinitionAddress);
                Logger.AssertMsg(cacheAddress.Type == BlamCacheAddressType.Definition, "INVALID CACHE ADDRESS");
                definitionReader.SeekTo(cacheAddress.Offset);
                StructureLayout layout = cacheFile.GetLayout("render geometry api resource definition");
                StructureValueCollection values = StructureReader.ReadStructure(definitionReader, layout);

                //Won't be useless eventually
                ulong numberOfUselessCrap3 = values.GetInteger("number of useless crap3");
                ulong numberOfUselessCrap4 = values.GetInteger("number of useless crap4");
                BlamCacheAddress uselessCrap3Address = new BlamCacheAddress((uint)values.GetInteger("address of useless crap3 table"));
                BlamCacheAddress uselessCrap4Address = new BlamCacheAddress((uint)values.GetInteger("address of useless crap4 table"));

                
                StructureLayout vertexBufferDefinitionLayout = new StructureLayout();
                vertexBufferDefinitionLayout.AddBasicField("vertex count", StructureValueType.Int32, 0x0);
                vertexBufferDefinitionLayout.AddBasicField("vertex format", StructureValueType.Int16, 0x4);
                vertexBufferDefinitionLayout.AddBasicField("vertex byte size", StructureValueType.Int16, 0x6);
                vertexBufferDefinitionLayout.AddBasicField("vertex buffer size", StructureValueType.Int32, 0x8);
                vertexBufferDefinitionLayout.AddBasicField("vertex buffer address", StructureValueType.UInt32, 0x14);
                //Vertex buffer data should be aligned to 0x4
                StructureLayout indexBufferDefinitionLayout = new StructureLayout();
                indexBufferDefinitionLayout.AddBasicField("index format", StructureValueType.Int16, 0x0);

                //Vertex buffer definitions
                //All of the vertex buffer definitions exist in a table of D3DPointer container structs
                //Each D3DPointer contains the address of the contained type (this is the only value we care about)
                //For example D3DPointer<Float> the first value might be 0x20000000 which would be an address containing some float

                VertexBufferDefinitions = new BlamD3DPointer<VertexBufferDefinition>[numberOfUselessCrap3];
                StructureLayout d3dStructureLayout = BlamD3DPointer<VertexBufferDefinition>.Layout();

                for (int i = 0; i < (int)numberOfUselessCrap3; i++)
                {
                    //Read each D3DPointer struct (Make a class for this too)
                    long offset = (i * d3dStructureLayout.Size);
                    long readAddr = uselessCrap3Address.Offset + offset;
                    definitionReader.SeekTo(readAddr);
                    StructureValueCollection d3dStructureValues = StructureReader.ReadStructure(definitionReader, d3dStructureLayout);

                    //Grab the pointer to the contained value (in our case it will be a VertexBufferDefinition)
                    BlamCacheAddress vertexBufferDefinitionAddress = new BlamCacheAddress((uint)d3dStructureValues.GetInteger("address"));
                    Logger.AssertMsg(vertexBufferDefinitionAddress.Type == BlamCacheAddressType.Definition, "Invalid vertex buffer definition address!");

                    //Read our vertex buffer definition (Make a class for this)
                    definitionReader.SeekTo(vertexBufferDefinitionAddress.Offset);
                    StructureValueCollection vertexBufferDefinitionValues = StructureReader.ReadStructure(definitionReader, vertexBufferDefinitionLayout);
                    //Grab our vertex buffer address
                    BlamCacheAddress vertexBufferAddress = new BlamCacheAddress((uint)vertexBufferDefinitionValues.GetInteger("vertex buffer address"));
                    EndianReader vertexBufferReader = null;
                    //Determine where to read the vertex data from
                    switch (vertexBufferAddress.Type)
                    {
                        case BlamCacheAddressType.Data: vertexBufferReader = primaryReader; break;
                        case BlamCacheAddressType.SecondaryData: vertexBufferReader = secondaryReader; break;
                        case BlamCacheAddressType.Definition: vertexBufferReader = definitionReader; break;
                    }
                    Logger.AssertMsg(vertexBufferReader != null, "INVALID VERTEX BUFFER ADDRESS");
                    //Need to make a class for this too, BUT the layout depends on what type of vertex it is...
                    //See https://github.com/MadJayQ/MCCEditor/blob/master/Blamite/Formats/Reach/Reach_VertexBuffer.xml
                    //This is for Reach but it's the best we got ATM...
                    StructureLayout vertexBufferLayout = new StructureLayout();
                    vertexBufferLayout.AddBasicField("posX", StructureValueType.Float32, 0x0);
                    vertexBufferLayout.AddBasicField("posY", StructureValueType.Float32, 0x4);
                    vertexBufferLayout.AddBasicField("posZ", StructureValueType.Float32, 0x8);
                    vertexBufferLayout.AddBasicField("posW", StructureValueType.Float32, 0x10);

                    //Read our vertex data buffer block
                    long vertexByteSize = (long)vertexBufferDefinitionValues.GetInteger("vertex byte size");
                    long vertexBufferTotalSize = (long)vertexBufferDefinitionValues.GetInteger("vertex buffer size");
                    long vertexCount = (long)vertexBufferDefinitionValues.GetInteger("vertex count");

                    Logger.AssertMsgFormat(vertexBufferTotalSize == (vertexByteSize * vertexCount), "Malformed vertex buffer, vertex byte size and vertex count do not add up to the total vertex buffer size! Total: {0}, Vertex Size: {1}, Vertex Count: {2}", vertexBufferTotalSize, vertexByteSize, vertexCount);

                    vertexBufferReader.SeekTo(vertexBufferAddress.Offset);
                    byte[] vertexBlock = vertexBufferReader.ReadBlock((int)vertexBufferDefinitionValues.GetInteger("vertex buffer size"));
                    using (EndianReader vertexBufferStreamReader = new EndianReader(new MemoryStream(vertexBlock), Endian.LittleEndian))
                    {

                        for (int vertexIdx = 0; vertexIdx < vertexCount; vertexIdx++)
                        {
                            long vertexBlockOffset = (vertexIdx * vertexByteSize);
                            StructureValueCollection vertexValues = StructureReader.ReadStructure(vertexBufferStreamReader, vertexBufferLayout);
                        }
                    }

                }
                //Index buffer definitions
                for (int i = 0; i < (int)numberOfUselessCrap4; i++)
                {
                    long offset = (i * 0xC);
                    long readAddr = uselessCrap4Address.Offset + offset;
                    definitionReader.SeekTo(readAddr);
                    StructureValueCollection d3dStructureValues = StructureReader.ReadStructure(definitionReader, d3dStructureLayout);

                    BlamCacheAddress indexBufferDefinitionAddress = new BlamCacheAddress((uint)d3dStructureValues.GetInteger("address"));
                    Logger.AssertMsg(indexBufferDefinitionAddress.Type == BlamCacheAddressType.Definition, "Invalid index buffer definition address!");

                    definitionReader.SeekTo(indexBufferDefinitionAddress.Offset);
                    StructureValueCollection indexBufferDefinitionValues = StructureReader.ReadStructure(definitionReader, indexBufferDefinitionLayout);
                }
            }
        }
    }
}
