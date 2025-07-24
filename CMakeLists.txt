cmake_minimum_required(VERSION 3.31)

project(Game LANGUAGES CXX)

add_executable(Game
        main.cpp
        gametree.h gametree.cpp
        fivenaction.h fivenaction.cpp
        node.h node.cpp
        surChessPieces.h
)

target_compile_features(Game PRIVATE cxx_std_23)







































#set(CMAKE_CXX_STANDARD_REQUIRED ON) #强制使用指定的C++标准（如 CMAKE_CXX_STANDARD 设置的版本），而不允许回退到较旧的标准。
# set(CMAKE_CXX_STANDARD 20)           # 希望使用 C++20
# set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 必须使用 C++20，不接受其他版本
# set(CMAKE_CXX_EXTENSIONS OFF) #避免编译器启用如g++的GNU扩展（如 -std=gnu++20），确保使用的是标准C++20（-std=c++20）。

# include(GNUInstallDirs)
# install(TARGETS Game
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# target_sources( Game
#     PRIVATE
#         main.cpp
#
#     PUBLIC
#
# )

#target_compile_options( Game PRIVATE -fmodules-ts )

#target_include_directories( Game PRIVATE /usr/include )

#target_link_directories( Game PRIVATE /usr/lib )

#target_link_libraries( Game PRIVATE m )

#set_target_properties( Game PROPERTIES
            #CXX_STANDARD 20
            #CXX_EXTENSIONS OFF
            #etc..
            #)
# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
#set_target_properties(Game PROPERTIES  CXX_STANDARD_REQUIRED on)

## --------------------------------------------------------------------------
## Adding build-requirements
## 源文件列表
#target_sources( <target> PRIVATE <source-file>... )
#
#target_precompile_headers( <target> PRIVATE <header-file>... )
#
## 预处理头文件搜索路径列表，等价参数：-I
#target_include_directories( <target> PRIVATE <include-search-dir>... )
#
## 预编译宏 自动在指定的宏前添加，等价参数：-D
#target_compile_definitions( <target> PRIVATE <macro-definitions>... )
#
## 编译选项和特性
#target_compile_options( <target> PRIVATE <compiler-option>... )
#target_compile_features( <target> PRIVATE <feature>... )
#
## 链接库
#target_link_libraries( <target> PRIVATE <dependency>... )
#
## 链接选项
#target_link_options( <target> PRIVATE <linker-option>... )
#
## 链接库搜索路径，等价参数：-L
#target_link_directories( <target> PRIVATE <linker-search-dir>... )

#execute_process(COMMAND ln -s /opt/gcc/gcm.cache  ./)

## Modern Cmake建议围绕Target和Property进行项目配置，竭力避免出现下面变量variable的set定义。
## Variable横行是典型CMake2.8时期的风格, Modern CMake则遵循了OO方法。其中，target表示待构建的对象，
## target相关的语句用于定义来约束link、compile等相关属性的scope。
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)


###完整的project命令
#project( Game LANGUAGES CXX )
         # DESCRIPTION "Description of project" HOMEPAGE_URL "http://www.abc.com" VERSION 0.1.0 )
#execute_process(COMMAND g++ -std=c++20 -xc++-system-header -fmodules-ts # 如何要使用modules,该命令应该放在project命令之后
#                iostream
#                string )
