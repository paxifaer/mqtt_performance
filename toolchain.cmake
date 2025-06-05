# 设置 CMake 的交叉编译工具链文件
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(TOOLCHAINPATH  /usr/bin)
# link_directories("${TOOLCHAINPATH}/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/aarch64-linux-gnu/lib")
# link_directories("${TOOLCHAINPATH}/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/x86_64-linux-gnu/lib")
# 设置交叉编译器和工具链路径


# 设置运行时搜索路径（-rpath）
# SET(CMAKE_INSTALL_RPATH "${TOOLCHAINPATH}/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/aarch64-linux-gnu/lib")

# 设置链接时搜索路径（-rpath-link）
# SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# set(CMAKE_C_COMPILER ${TOOLCHAINPATH}/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc)
# set(CMAKE_CXX_COMPILER ${TOOLCHAINPATH}/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-g++)
# set(CMAKE_LINKER ${TOOLCHAINPATH}/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-ld)
# set(CMAKE_STRIP ${TOOLCHAINPATH}/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-strip)

set(CMAKE_C_COMPILER ${TOOLCHAINPATH}/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAINPATH}/aarch64-linux-gnu-g++)
set(CMAKE_LINKER ${TOOLCHAINPATH}/aarch64-linux-gnu-ld)
set(CMAKE_STRIP ${TOOLCHAINPATH}/aarch64-linux-gnu-strip)
# set(CMAKE_C_COMPILER_WORKS   1)
# set(CMAKE_CXX_COMPILER_WORKS 1) 
# 设置 CMake 编译选项
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# 设置编译器和链接器的参数
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

# 设置交叉编译器头文件和库文件路径
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAINPATH}/include)
