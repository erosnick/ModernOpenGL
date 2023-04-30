using System;
using System.Runtime.CompilerServices;

namespace Aria
{
    public class Component
    {
        private IntPtr handle = (IntPtr)0;

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Component[] GetComponents();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int GetInternalId(IntPtr handle);

        public int ID
        {
            get
            {
                return GetInternalId(handle);
            }
        }

        public int Tag
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }
    }
}
