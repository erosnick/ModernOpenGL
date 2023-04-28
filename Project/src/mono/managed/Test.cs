using System;

namespace ClassLibrary
{
    public class Test
    {
        public Test()
        {
            Console.WriteLine("Test constructor.");
        }

        public void printMessage()
        {
            Console.WriteLine("Hello world from C#.");
        }

        public void printCustomMessageString(string message) 
        {
            Console.WriteLine($"C# says: {message}");
        }

        public void printCustomMessageInt(int value)
        {
            Console.WriteLine($"C# says: {value}");
        }
    }
}
