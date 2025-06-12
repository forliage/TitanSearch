# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 项目定义
project(TitanSearch LANGUAGES CXX VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- 寻找和集成依赖库 (手动方式) ---

# 提示：请将第三方库源码放置在 third_party 目录下
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

# 1. 对于简单的、或头文件为主的库，我们直接使用 add_subdirectory
# --------------------------------------------------------------------
# Gumbo-Parser (HTML解析)
# 假设源码在 third_party/gumbo-parser
add_subdirectory(${THIRD_PARTY_DIR}/gumbo-parser)

# Jieba-CPP (中文分词)
# 假设源码在 third_party/cpp-jieba
# Jieba-CPP 需要一个头文件目录
include_directories(${THIRD_PARTY_DIR}/cpp-jieba/include)
add_subdirectory(${THIRD_PARTY_DIR}/cpp-jieba/deps/limonp) # Jieba的依赖
add_subdirectory(${THIRD_PARTY_DIR}/cpp-jieba)


# 2. 对于需要预编译的复杂库，我们使用 find_package
#    请在运行 cmake 时使用 -DCMAKE_PREFIX_PATH=<your_install_dir>
# --------------------------------------------------------------------
find_package(CURL REQUIRED)
find_package(RocksDB REQUIRED)
find_package(Protobuf REQUIRED)


# --- 处理 Protocol Buffers ---
# 确保 protoc 编译器在系统 PATH 中
set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
file(GLOB PROTO_FILES "${PROTO_DIR}/*.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})


# --- 定义头文件和源文件 ---
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR} # Protobuf 生成的头文件会在这里
)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")


# --- 创建核心库 ---
add_library(TitanSearchLib STATIC
    ${SRC_FILES}
    ${PROTO_SRCS}
)

# 为核心库链接所有依赖
target_link_libraries(TitanSearchLib PUBLIC
    # 从 add_subdirectory 添加的库
    gumbo       # gumbo-parser 目标
    cppjieba    # Jieba-CPP 目标

    # 从 find_package 找到的库
    CURL::libcurl
    rocksdb::rocksdb
    protobuf::libprotobuf
)

# --- 创建可执行文件 ---
add_executable(TitanSearchApp src/main.cpp)

# 链接我们的核心库到主程序
target_link_libraries(TitanSearchApp PRIVATE TitanSearchLib)

# 在Windows上，运行时可能需要dll，将它们复制到输出目录
if(WIN32)
    add_custom_command(TARGET TitanSearchApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:CURL::libcurl>"
        $<TARGET_FILE_DIR:TitanSearchApp>
    )
    # 为 RocksDB 和其他DLL也添加类似命令
endif()


# --- 输出提示信息 ---
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Build mode: Manual dependency management")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Looking for pre-built libraries in CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

# --- VSCode 推荐配置 ---
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)