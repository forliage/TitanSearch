# TitanSearch (泰坦搜索)

一个使用 C++ 从零开始构建的、包含自定义数据库的大规模搜索引擎项目。

## 核心特性

*   **多线程网络爬虫**: 高效抓取网页数据。
*   **中文分词与索引**: 支持中文内容，构建倒排索引。
*   **自定义存储引擎**: 基于 RocksDB 构建高性能的持久化索引存储。
*   **相关性排序查询**: 实现 BM25 等经典排序算法，提供高质量搜索结果。

## 技术栈

*   **语言**: C++17
*   **构建系统**: CMake
*   **包管理器**: [vcpkg](https://github.com/microsoft/vcpkg) (强烈推荐)
*   **核心依赖**:
    *   `libcurl`: 网络请求
    *   `gumbo-parser`: HTML 解析
    *   `Jieba-CPP`: 中文分词
    *   `RocksDB`: KV 存储引擎
    *   `Protocol Buffers`: 数据序列化

## 环境准备与依赖安装

在开始之前，请确保你已安装 C++ 编译器 (如 MSVC, GCC, Clang)，CMake, 和 Git。

我们强烈建议使用 `vcpkg` 来管理所有第三方依赖。这将在 Windows 上极大地简化配置过程。

1.  **安装 vcpkg**:
    ```bash
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.bat
    ```

2.  **安装项目依赖**:
    ```bash
    # 对于64位Windows
    ./vcpkg install rocksdb:x64-windows protobuf:x64-windows libcurl:x64-windows gumbo-parser:x64-windows
    ```
    *注意：`Jieba-CPP` 可能需要手动集成或通过 vcpkg 的自定义 port 安装。为简化起步，你也可以先将其源码放入 `third_party` 目录。*

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
    运行 CMake 配置命令，并指定 vcpkg 的工具链文件。**这是关键步骤！**
    ```bash
    # 将 <path\to\vcpkg> 替换为你的 vcpkg 实际路径
    cmake .. -DCMAKE_TOOLCHAIN_FILE=<path\to\vcpkg>/scripts/buildsystems/vcpkg.cmake
    ```
    *VSCode 用户：可以在 `.vscode/settings.json` 中配置此项，之后 VSCode 的 CMake 插件会自动使用。*

3.  **编译项目**:
    ```bash
    cmake --build . --config Release
    ```

4.  **运行**:
    编译成功后，可执行文件将位于 `build/Release/` 目录下。
    ```bash
    ./Release/TitanSearchApp.exe
    ```

## 开发路线图

- [ ] **第一阶段**: 最小可用原型 (内存版)
- [ ] **第二阶段**: 持久化与并发
- [ ] **第三阶段**: 功能完善与性能优化
- [ ] **第四阶段**: 分布式扩展