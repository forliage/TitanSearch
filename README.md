# TitanSearch (泰坦搜索)

一个使用 C++ 从零开始构建的、包含自定义数据库的大规模搜索引擎项目。(手动依赖管理版)

## 核心特性

*   **多线程网络爬虫**: 高效抓取网页数据。
*   **中文分词与索引**: 支持中文内容，构建倒排索引。
*   **自定义存储引擎**: 基于 RocksDB 构建高性能的持久化索引存储。
*   **相关性排序查询**: 实现 BM25 等经典排序算法，提供高质量搜索结果。

## 技术栈

*   **语言**: C++17
*   **构建系统**: CMake
*   **核心依赖**:
    *   `libcurl`: 网络请求
    *   `gumbo-parser`: HTML 解析
    *   `Jieba-CPP`: 中文分词
    *   `RocksDB`: KV 存储引擎
    *   `Protocol Buffers`: 数据序列化

## 环境准备与依赖安装 (手动)

**重要提示**: 手动管理 C++ 依赖比使用包管理器复杂。你需要确保下载的库版本兼容，并正确编译它们（特别是动态库与静态库，Debug 与 Release 版本的匹配）。

以下是推荐的设置步骤：

1.  **下载依赖库**:
    访问以下链接，下载所有库的源码或针对你编译器的预编译包 (如果提供的话)。将它们解压到 `third_party` 目录下。
    *   **RocksDB**: [https://github.com/facebook/rocksdb](https://github.com/facebook/rocksdb)
    *   **Protocol Buffers**: [https://github.com/protocolbuffers/protobuf](https://github.com/protocolbuffers/protobuf)
    *   **libcurl**: [https://curl.se/download.html](https://curl.se/download.html)
    *   **gumbo-parser**: [https://github.com/google/gumbo-parser](https://github.com/google/gumbo-parser)
    *   **Jieba-CPP**: [https://github.com/yanyiwu/cpp-jieba](https://github.com/yanyiwu/cpp-jieba)

2.  **编译和安装依赖**:
    *   **对于 `gumbo-parser` 和 `Jieba-CPP`**: 这两个库比较简单，我们的 `CMakeLists.txt` 将会尝试直接将它们的源码作为项目的一部分进行编译，你只需将源码放在 `third_party` 目录即可。
    *   **对于 `libcurl`, `RocksDB`, `Protobuf`**: 这三个库更为复杂，建议单独编译它们。
        1.  **Protobuf**: 你需要编译 `protoc` 编译器和 `protobuf` 库。编译完成后，确保 `protoc.exe` 所在的 `bin` 目录被添加到了系统的 `PATH` 环境变量中。
        2.  **RocksDB & libcurl**: 按照它们各自官方文档的指引进行编译。
        3.  **安装位置**: 编译完成后，你会得到头文件 (`.h`, `.hpp`)，库文件 (`.lib`, `.dll` 或 `.a`, `.so`)。建议将所有这些库的产出物统一安装到一个地方，例如 `C:/local/`，目录结构如下：
            ```
            C:/local/
            ├── include/  (存放所有库的头文件)
            ├── lib/      (存放所有库的.lib或.a文件)
            └── bin/      (存放所有库的.dll或.so文件)
            ```
            这样做便于 CMake 查找。

## 编译与运行

1.  **克隆项目**:
    ```bash
    git clone <你的仓库URL>
    cd TitanSearch
    ```

2.  **配置 CMake**:
    在 `TitanSearch` 根目录下创建 `build` 文件夹并进入。
    ```bash
    mkdir build
    cd build
    ```
    运行 CMake 配置命令。你需要通过 `-DCMAKE_PREFIX_PATH` 告诉 CMake 你把编译好的依赖安装在了哪里。
    ```bash
    # 将 "C:/local" 替换为你的实际安装路径
    cmake .. -DCMAKE_PREFIX_PATH="C:/local"
    ```
    *如果 CMake 仍然找不到某个库，你可能需要手动设置更具体的变量，例如 `-DROCKSDB_DIR=...`*

3.  **编译项目**:
    ```bash
    cmake --build . --config Release
    ```

4.  **运行**:
    编译成功后，可执行文件将位于 `build/Release/` 目录下。
    *在 Windows 上，你可能需要将依赖的 `.dll` 文件 (如 `libcurl.dll`, `rocksdb.dll`) 复制到可执行文件旁边才能运行。*
    ```bash
    ./Release/TitanSearchApp.exe
    ```