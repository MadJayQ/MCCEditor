using Blamite.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace HaloMCCEditor.Source.Blam.Serialization
{
    public interface IBlamCacheFileStructure
    {
        object DeserializeFromFile(IReader reader);
    }

    public class BlamCacheFileSerializer
    {
        private static BlamCacheFileSerializer _instance;
        public BlamCacheFileSerializer Instance 
        {
            get
            {
                if (_instance == null) _instance = new BlamCacheFileSerializer();
                return _instance;
            }
        }


    }

}
