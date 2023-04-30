using System;
using System.Runtime.CompilerServices;

namespace Aria
{
    public class Test
    {
        public struct Vector3
        {
            public float x;
            public float y;
            public float z;

            public  Vector3(float x, float y, float z)
            {
                this.x = x;
                this.y = y;
                this.z = z;
            }
        }

        public Test()
        {
            Console.WriteLine("Test constructor.");
        }

        public void printMessage()
        {
            Console.WriteLine("Hello world from C#.");
            Console.WriteLine(greetings());
            nativeLog("Hello world from C#", 2333);

            Vector3 parameter = new Vector3(1.0f, 0.0f, 0.0f);
            nativeLogVector3(ref parameter);

            Vector3 result;
            nativeLogVector3WithReturnValue(ref parameter, out result);

            Console.WriteLine("{0}, {1}, {2}", result.x, result.y, result.z);
        }

        public void printCustomMessageString(string message) 
        {
            Console.WriteLine($"C# says: {message}");
        }

        public void printCustomMessageInt(int value)
        {
            Console.WriteLine($"C# says: {value}");
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern string greetings();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void nativeLog(string message, int value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void nativeLogVector3(ref Vector3 parameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void nativeLogVector3WithReturnValue(ref Vector3 parameter, out Vector3 result);
    }
}
