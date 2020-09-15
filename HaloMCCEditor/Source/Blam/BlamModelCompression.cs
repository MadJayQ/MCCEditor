using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Blamite.Blam;
using Blamite.Blam.Resources.Models;
using Blamite.IO;
using Blamite.Serialization;

namespace HaloMCCEditor.Core.Blam
{
    [Flags]
    public enum RenderGeometryCompressionFlags : ushort
    {
        None = 0,
        CompressedPosition = 1 << 0,
        CompressedTexcoord = 1 << 1,
        CompressionOptimized = 1 << 2
    }

    public class BlamModelCompression
    {
        public class BlamModelCompressionElement
        {
            public RenderGeometryCompressionFlags CompressionFlags { get; private set; }
            public BoundingBox CompressionBoundingBox { get; private set; }

            public BlamModelCompressionElement(StructureValueCollection elementData)
            {
                CompressionFlags = (RenderGeometryCompressionFlags)elementData.GetInteger("compression flags");
                CompressionBoundingBox = new BoundingBox();

                CompressionBoundingBox.MinX = elementData.GetFloat("min x");
                CompressionBoundingBox.MaxX = elementData.GetFloat("max x");
                CompressionBoundingBox.MinY = elementData.GetFloat("min y");
                CompressionBoundingBox.MaxY = elementData.GetFloat("max y");
                CompressionBoundingBox.MinZ = elementData.GetFloat("min z");
                CompressionBoundingBox.MaxZ = elementData.GetFloat("max z");
                CompressionBoundingBox.MinU = elementData.GetFloat("min u");
                CompressionBoundingBox.MaxU = elementData.GetFloat("max u");
                CompressionBoundingBox.MinV = elementData.GetFloat("min v");
                CompressionBoundingBox.MaxV = elementData.GetFloat("max z");
            }
        }

        public List<BlamModelCompressionElement> CompressionElements { get; private set; }

        public BlamModelCompression()
        {
            CompressionElements = new List<BlamModelCompressionElement>();
        }

        public void ReadCompressionData(List<StructureValueCollection> boundingBoxTable)
        {
            foreach(StructureValueCollection bbox in boundingBoxTable)
            {
                CompressionElements.Add(new BlamModelCompressionElement(bbox));
            }

        }

    }
}
