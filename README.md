# TitanSearch (泰坦搜索)

一个使用现代 C++ 从零开始构建的、包含功能强大的自定义存储引擎的大规模搜索引擎项目。

## 项目愿景

本项目旨在深入探索和实践大规模数据处理的核心技术。我们不仅仅是调用API，而是深入底层，亲手构建一个搜索引擎的四大支柱：**爬虫、索引、存储和查询**。项目的核心挑战与亮点在于设计并实现一个专为搜索场景优化的、基于 RocksDB 的高性能持久化存储系统。

## 架构概览

系统由四个解耦的子系统构成，它们通过定义良好的数据接口进行协作：

  <!-- 这是一个示例图，你可以自己画一个更详细的 -->

1.  **爬虫系统 (Crawler)**: 负责从互联网上抓取海量网页，作为数据源。
2.  **索引系统 (Indexer)**: 解析网页，提取文本，分词，并构建核心数据结构——倒排索引和正排索引。
3.  **存储系统 (Storage Engine)**: **本项目的核心**。将索引数据进行序列化，并利用 RocksDB 的高级特性（如列族、事务）进行高效、可靠的持久化存储。
4.  **查询系统 (Querying Engine)**: 接收用户查询，从存储系统中检索相关文档，利用 BM25 等算法进行排序，并生成用户友好的搜索结果。

---

## 环境准备与构建指南 (Windows + VSCode)

本指南将带你从零开始，一步步搭建完整的开发环境。

### **第一步：安装必备工具**

在开始之前，请确保你的系统上已安装以下软件：

1.  **Git**: 用于版本控制和下载 vcpkg。 [下载地址](https://git-scm.com/download/win)
2.  **CMake**: C++ 的跨平台构建系统。 [下载地址](https://cmake.org/download/) (安装时请勾选 "Add CMake to the system PATH for all users")
3.  **Visual Studio 2019 或更高版本**: 需要其附带的 MSVC C++ 编译器。安装时，请确保选中 "使用C++的桌面开发" 工作负载。

### **第二步：安装 vcpkg 及项目依赖**

`vcpkg` 是微软官方的 C/C++ 包管理器，它能奇迹般地简化在 Windows 上的依赖管理。

1.  **下载并安装 vcpkg**:
    打开一个 PowerShell 或 CMD 终端，执行以下命令：
    ```bash
    # 建议将 vcpkg 安装在一个稳定的位置，例如 C:\dev\vcpkg
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    .\bootstrap-vcpkg.bat
    ```
    等待脚本执行完毕。

2.  **安装项目所有依赖**:
    在 vcpkg 目录下，执行以下命令安装 `TitanSearch` 所需的所有库。这可能需要一些时间。
    ```bash
    # 我们为 64 位 Windows 系统安装这些库
    .\vcpkg.exe install rocksdb:x64-windows protobuf:x64-windows libcurl:x64-windows gumbo-parser:x64-windows
    ```
    *   **rocksdb**: 我们自定义数据库的物理存储层。
    *   **protobuf**: 用于高效的数据序列化/反序列化。
    *   **libcurl**: 用于爬虫模块下载网页。
    *   **gumbo-parser**: 用于解析 HTML 文档。
    *   **关于 `Jieba-CPP`**: vcpkg 可能没有最新的 `Jieba-CPP`。初期，你可以直接将 [Jieba-CPP 的源码](https://github.com/yanyiwu/cppjieba) 下载并放入 `third_party` 目录，通过 CMake 的 `add_subdirectory()` 手动集成。

### **第三步：配置 VSCode 并构建项目**

1.  **安装 VSCode 扩展**:
    在 VSCode 中，安装 `C/C++` 和 `CMake Tools` 这两个核心扩展。

2.  **克隆本项目仓库**:
    ```bash
    git clone <你的 TitanSearch 仓库URL>
    cd TitanSearch
    ```

3.  **配置 VSCode 与 vcpkg 的联动 (关键步骤!)**:
    *   打开 `TitanSearch` 项目文件夹。
    *   按下 `Ctrl+Shift+P` 打开命令面板，输入 `CMake: Select a Kit`，选择一个 Visual Studio 的编译器（如 `Visual C++ Build Tools ... - amd64`）。
    *   在项目根目录下找到 `.vscode/settings.json` 文件 (如果没有则创建)，并添加以下内容，**将路径替换为你的 vcpkg 实际路径**：
        ```json
        {
            "cmake.configureSettings": {
                "CMAKE_TOOLCHAIN_FILE": "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"
            }
        }
        ```
    *   此时，`CMake Tools` 扩展会自动检测到配置变更并开始配置项目。你可以在 VSCode 的 "输出" 面板中看到 CMake 的日志。

4.  **编译与运行**:
    *   **编译**: 按下 `F7` 键，或点击 VSCode 底部状态栏的 `Build` 按钮。
    *   **运行**: 编译成功后，点击状态栏中的 `▶` (Launch) 按钮来运行 `TitanSearchApp`。

---

## **核心设计：功能强大的存储引擎**

我们的“自定义数据库”并非从零手写 B+树，而是基于工业级 KV 存储引擎 `RocksDB` 构建一个**为搜索引擎场景深度优化的逻辑数据层**。这兼顾了可行性与项目的技术深度。

### **1. 核心理念：列族 (Column Families)**

RocksDB 的“列族”是实现高性能的关键。它允许我们将不同类型的数据存储在逻辑上独立的区域，每个区域可以有自己独立的内存缓存（MemTable）、SST 文件和压缩算法。这对于我们的搜索引擎至关重要。

我们将规划以下列族：

*   `CF_INVERTED_INDEX`: 存储倒排索引。数据量最大，写操作频繁。
*   `CF_FORWARD_INDEX`: 存储正排索引。用于获取文档快照和元信息。
*   `CF_METADATA`: 存储系统元数据，如 URL 到文档 ID 的映射、总文档数等。
*   `CF_PAGERANK`: (可选高级功能) 存储离线计算好的 PageRank 分数。

### **2. 数据模型与序列化 (Protocol Buffers)**

我们使用 Protocol Buffers (Protobuf) 来定义数据结构，以实现高效、跨平台的序列化。

**在 `proto/document.proto` 中定义:**
```protobuf
syntax = "proto3";

package titiansearch;

// 单个倒排列表项
message Posting {
  uint64 doc_id = 1;         // 文档ID
  uint32 term_frequency = 2; // 词频 TF
  repeated uint32 positions = 3; // 词元在文档中的位置 (用于短语查询)
}

// 完整的倒排列表 (一个词元对应一个)
message PostingList {
  repeated Posting postings = 1;
}

// 正排索引中的文档信息
message DocumentInfo {
  string url = 1;
  string title = 2;
  string snapshot = 3; // 网页摘要
  uint32 doc_length = 4; // 文档长度 (用于BM25计算)
  double page_rank = 5;  // 静态质量分
}
```

### **3. Key-Value  Schema 设计**

我们通过精心设计的 Key 格式，在 KV 存储中模拟结构化数据。

| 列族 (Column Family) | Key 格式                | Value 格式                        | 说明                               |
| -------------------- | ----------------------- | --------------------------------- | ---------------------------------- |
| `CF_INVERTED_INDEX`  | `[term]` (词元)         | `Serialized<PostingList>`         | **核心**: 词元 -> 文档列表         |
| `CF_FORWARD_INDEX`   | `[doc_id]` (二进制)     | `Serialized<DocumentInfo>`        | **核心**: 文档ID -> 文档详细信息   |
| `CF_METADATA`        | `url:[url_string]`      | `[doc_id]` (二进制)               | URL 到 DocID 的快速映射            |
| `CF_METADATA`        | `sys:total_docs`        | `[count]` (二进制)                | 系统总文档数，用于 IDF 计算        |
| `CF_METADATA`        | `sys:last_doc_id`       | `[id]` (二进制)                   | 全局自增文档 ID                    |
| `CF_PAGERANK`        | `[doc_id]` (二进制)     | `[pagerank_score]` (double二进制) | 存储 PageRank 分数                 |

*注：`[value]` 表示原始二进制值，`Serialized<T>` 表示 Protobuf 序列化后的字节流。*

### **4. 关键操作与事务性保证**

**索引一篇新文档是一个原子操作**，必须保证数据一致性。例如，不能出现倒排索引更新了，而正排索引没更新的情况。

我们将使用 RocksDB 的 `WriteBatch` 来实现事务性写入：

```cpp
// 伪代码示例
void Indexer::addDocument(const ParsedPage& page) {
    // 1. 分配新 DocID (原子性增加 sys:last_doc_id)
    uint64_t newDocId = storage->getNextDocId();

    // 2. 准备数据
    DocumentInfo docInfo = createDocumentInfo(page, newDocId);
    auto serializedDocInfo = serialize(docInfo);

    // 3. 开始一个事务 (WriteBatch)
    rocksdb::WriteBatch batch;

    // 4. 加入正排索引写入
    batch.Put(CF_FORWARD_INDEX, to_slice(newDocId), serializedDocInfo);

    // 5. 加入 URL -> DocID 映射
    batch.Put(CF_METADATA, "url:" + page.url, to_slice(newDocId));

    // 6. 遍历所有词元，更新倒排索引
    for (const auto& term : page.terms) {
        // a. 读取旧的 PostingList
        PostingList oldList = storage->getPostingList(term);
        // b. 创建新的 Posting 并加入 List
        Posting newPosting = createPosting(newDocId, ...);
        oldList.add(newPosting);
        // c. 加入倒排索引的更新
        batch.Put(CF_INVERTED_INDEX, term, serialize(oldList));
    }
    
    // 7. 原子性地提交所有写入操作
    storage->write(batch);
}
```
这种设计确保了每次索引操作的**原子性**和**一致性**，这是“功能强大”的数据库必须具备的特性。

---

## 开发路线图

- [ ] **第一阶段: MVP (最小可用原型)**
    - [ ] 完成单线程爬虫和 HTML 解析
    - [ ] 实现内存中的倒排、正排索引 (`std::unordered_map`)
    - [ ] 实现 Jieba-CPP 中文分词集成
    - [ ] 实现基于 TF-IDF 的命令行查询工具

- [ ] **第二阶段: 持久化与并发**
    - [ ] **实现 `StorageManager` 类，封装 RocksDB 操作**
    - [ ] **集成 Protobuf，完成数据序列化**
    - [ ] **实现上述的列族和 Key-Value Schema 设计**
    - [ ] **使用 `WriteBatch` 实现原子化索引更新**
    - [ ] 将爬虫改造为多线程模型

- [ ] **第三阶段: 功能完善与性能优化**
    - [ ] 实现 **BM25 排序算法**，替换 TF-IDF
    - [ ] 支持短语查询 (利用 `Posting` 中的 `positions` 信息)
    - [ ] (高级) 实现一个简单的 PageRank 离线计算器
    - [ ] 搭建基于 `cpp-httplib` 的简单 Web API 接口
    - [ ] 为 RocksDB 调优，配置缓存大小和压缩算法

- [ ] **第四阶段: 分布式探索 (远期)**
    - [ ] 研究索引分片 (Sharding) 方案
    - [ ] 设计查询聚合器 (Query Aggregator)
    - [ ] 设计分布式爬虫管理