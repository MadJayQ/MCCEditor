using Blamite.IO;
using Blamite.Serialization;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HaloMCCEditor.Core.Blam.Resources
{
    public class BlamD3DPointer<T>
    {
        public T Instance { get; private set; }
        public BlamCacheAddress Address { get; private set; }

        public BlamD3DPointer(StructureValueCollection values)
        {
            Instance = Activator.CreateInstance<T>();
            Address = new BlamCacheAddress((uint)values.GetInteger("address"));
        }

        public static StructureLayout Layout()
        {
            StructureLayout d3dStructureLayout = new StructureLayout(0xC);
            d3dStructureLayout.AddBasicField("address", StructureValueType.UInt32, 0x0);
            d3dStructureLayout.AddBasicField("definition address", StructureValueType.UInt32, 0x4);
            d3dStructureLayout.AddBasicField("runtime address", StructureValueType.UInt32, 0x8);

            return d3dStructureLayout;
        }
    }
}
