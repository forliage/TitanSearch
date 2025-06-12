# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 项目定义
project(TitanSearch LANGUAGES CXX VERSION 1.0)

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- 寻找依赖库 ---
# vcpkg 会帮助 CMake 找到这些库
find_package(CURL REQUIRED)
find_package(Gumbo REQUIRED)
find_package(RocksDB REQUIRED)
find_package(Protobuf REQUIRED)

# 如果 Jieba-CPP 不在 vcpkg 中, 你可以暂时将其作为子模块或源码引入
# add_subdirectory(third_party/Jieba-CPP) # 示例

# --- 处理 Protocol Buffers ---
# 定义 proto 文件存放目录
set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
# 找到所有 .proto 文件
file(GLOB PROTO_FILES "${PROTO_DIR}/*.proto")

# 生成 C++ 源文件和头文件
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# --- 定义头文件和源文件 ---

# 定义公共头文件目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR} # Protobuf 生成的头文件会在这里
)

# 搜集所有源文件
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# --- 创建核心库 ---
# 将所有功能模块编译成一个静态库，便于管理
add_library(TitanSearchLib STATIC
    ${SRC_FILES}
    ${PROTO_SRCS} # 添加 protobuf 生成的源文件
)

# 为核心库链接依赖
target_link_libraries(TitanSearchLib PUBLIC
    CURL::libcurl       # libcurl
    gumbo               # gumbo-parser
    rocksdb::rocksdb    # RocksDB
    protobuf::libprotobuf # Protobuf
    # Jieba-CPP         # 如果使用 add_subdirectory 添加
)

# --- 创建可执行文件 ---
add_executable(TitanSearchApp src/main.cpp)

# 链接我们的核心库到主程序
target_link_libraries(TitanSearchApp PRIVATE TitanSearchLib)

# --- 输出提示信息 ---
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Found CURL: ${CURL_LIBRARIES}")
message(STATUS "Found RocksDB: ${RocksDB_LIBRARIES}")
message(STATUS "Found Protobuf: ${Protobuf_LIBRARIES}")

# --- VSCode 推荐配置 ---
# 为了让 VSCode 的 IntelliSense 正常工作
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)