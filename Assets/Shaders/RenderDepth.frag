#version 460 core

void main()
{
  // We could explicitly set the depth by uncommenting its one line,but this is
  // effectively what happens behind the scene anyways.
  // It is a very bad idea to write anything to it in a shader though,
  // because it breaks a lot of hardware depth optimizations(Hi-Z,early depth tests,etc...)
  // gl_FragDepth = gl_FragCoord.z;
}
