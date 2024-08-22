# Toolchain-mingw64.cmake

# Target system
set(CMAKE_SYSTEM_NAME Windows)

# Cross compiler to use for C, C++ and for generating resource files
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Target architecture
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# where is the target environment
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# adjust the default behavior of the CMake try_compile() function
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
