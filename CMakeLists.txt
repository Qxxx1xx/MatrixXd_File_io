# 指定最低所需的 CMake 版本
cmake_minimum_required(VERSION 3.15)

# 设置 C++ 标准: CMAKE_CXX_STANDARD 变量
# 整数变量，需要 C++17 设为 17，需要 C++23 设为 23
set(CMAKE_CXX_STANDARD 17)
# CMAKE_CXX_STANDARD_REQUIRED 变量为 BOOL 类型，选项为 ON 和 OFF，默认 OFF
# 表示是否一定要支持指定的 C++ 标准，OFF 表示如不支持 C++17 则降为 C++14 或者更加低， ON 表示不支持则报错
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# CMAKE_CXX_EXTENSIONS 变量为 BOOL 类型，默认为 ON
# 设为 ON 表示启用 GCC 特有的扩展功能，设为 OFF 表示关闭 GCC 的扩展功能，只使用标准的 C++
# 我们项目需要大量用到 MSVC 编译器，则设为 OFF 
set(CMAKE_CXX_EXTENSIONS OFF)
# 以上变量设置在 project 之前，project 函数会对编译器做一些检测


# 初始化项目信息，并把当前 CMakeList.txt所在位置作为根目录
# LANGUAGES 选项设置语言
project(usingEigen LANGUAGES CXX VERSION 0.1.2)
# VERSION 0.1.2 把当前版本号设定为 0.1.2 依次表示主版本号、次版本号和补丁版本号

# 将构建模式默认值设置为 Releace
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Releace)
endif()

# 不再定义 DNOMINMAX 宏
if (WIN32)
    add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
endif()

# 使编译具有缓存，编译更加快
if (NOT MSVC)
    find_program(CCACHE_PROGRAM ccache)
    if (CCACHE_PROGRAM)
        massage(STATUS "Found CCache: ${CCACHE_PROGRAM}")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_PROGRAM})
    endif()
endif()


add_executable(${PROJECT_NAME} main.cpp)
set(LIBPACK_DIR D:/MSTMMSim/LibPack)
set(INCLUDE_LISTS 
    ${LIBPACK_DIR}/include/eigen3
    )
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_LISTS})
