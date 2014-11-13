solution "Assignment0"
  configurations { "Debug", "Release" }

  configuration { "Debug" }
    targetdir "debug"
    buildoptions{"-std=c++0x", "-fpermissive"}

  configuration { "Release" }
    targetdir "release"
    buildoptions{"-std=c++0x", "-fpermissive"}

  --another solution
  --include("./include/vecmath")

project "Assignment0"
  language "C++"
  kind     "ConsoleApp"
  files  { "main.cpp","./vecmath/*.cpp" }
  includedirs {"./include/vecmath"}
  --links {"GL","GLU","glut","vecmath"}
  links {"GL","GLU","glut"}

  configuration { "Debug*" }
    defines { "_DEBUG", "DEBUG" }
    flags   { "Symbols" }

  configuration { "Release*" }
    defines { "NDEBUG" }
    flags   { "Optimize" }
