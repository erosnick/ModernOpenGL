using System;

namespace Aria
{
    public static class Main
    {
        public static void TestComponent()
        {
            Component[] components = Component.GetComponents();

            foreach(Component component in components)
            {
                Console.WriteLine("Component id is " + component.ID);
                Console.WriteLine("Component tag is " + component.Tag);
            }
        }
    }
}
