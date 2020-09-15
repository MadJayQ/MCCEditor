using Blamite.Serialization;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HaloMCCEditor.Core.Blam.Serialization
{
    public class BlamStructureElementAttribute : System.Attribute
    {
        public BlamStructureElementAttribute(StructureValueType valueType, string xmlValueName)
        {

        }
    }
}
