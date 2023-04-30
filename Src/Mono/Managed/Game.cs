using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Aria
{
    public class Game : IGame
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void InitGLFW();

        public void Init()
        {
            InitGLFW();
            Console.WriteLine("Game.Foo()");
        }
    }
}
