﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Aria
{
    public class GameMain
    {
        public static IGame CreateGame()
        {
            return new Game();
        }
    }
}
