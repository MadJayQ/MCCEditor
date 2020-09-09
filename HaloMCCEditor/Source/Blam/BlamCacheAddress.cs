using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HaloMCCEditor.Core.Blam
{

    public enum BlamCacheAddressType
    {
        Memory,
        Definition,
        Data,
        SecondaryData
    }


    public struct BlamCacheAddress : IComparable<BlamCacheAddress>, IEquatable<BlamCacheAddress>
    {
        private const int TypeShift = 29;
        private const uint OffsetMask = (uint.MaxValue >> (32 - TypeShift));

        public uint Value { get; set; }

        public BlamCacheAddressType Type => (BlamCacheAddressType)(Value >> TypeShift);

        public int Offset => (int)(Value & OffsetMask);

        public static explicit operator int(BlamCacheAddress address) => address.Offset;

        public BlamCacheAddress(uint value) { Value = value; }

        public BlamCacheAddress(BlamCacheAddressType type, int offset) : this((((uint)type) << TypeShift) | ((uint)offset & OffsetMask)) { }

        public bool Equals(BlamCacheAddress other) => Value == other.Value;

        public override bool Equals(object obj) => obj is BlamCacheAddress ? Equals((BlamCacheAddress)obj) : false;

        public static bool operator ==(BlamCacheAddress a, BlamCacheAddress b) => a.Equals(b);

        public static bool operator !=(BlamCacheAddress a, BlamCacheAddress b) => !a.Equals(b);

        public override int GetHashCode() => Value.GetHashCode();

        public int CompareTo(BlamCacheAddress other) => Value.CompareTo(other.Value);

        public override string ToString() => $"{{ Type: {Type}, Offset: {Offset} }}";

    }
}
