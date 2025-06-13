#include <iostream>
#include <string>
#include <vector>
#include <memory>

// --- 项目核心头文件 ---
// 注意：这些头文件现在还不存在，需要你根据项目结构创建
#include "titansearch/common/utils.h"
#include "titansearch/crawler/crawler.h"
#include "titansearch/indexing/indexer.h"
#include "titansearch/storage/storage_manager.h"
#include "titansearch/querying/query_processor.h"

// 伪造的命令行参数解析函数
void parse_arguments(int argc, char* argv[], std::string& mode) {
    if (argc > 1) {
        mode = std::string(argv[1]);
    } else {
        mode = "interactive"; // 默认为交互模式
    }
}

// 爬取和索引模式
void run_crawl_and_index_mode() {
    std::cout << "=======================================\n";
    std::cout << " TitanSearch: Crawl & Index Mode \n";
    std::cout << "=======================================\n\n";

    // TODO: 从配置文件或命令行读取配置
    std::vector<std::string> seed_urls = {"http://example.com", "http://example.org"};
    std::string db_path = "./data/index";

    // 1. 初始化存储管理器
    // StorageManager 将是封装 RocksDB 的核心类
    std::cout << "[1/4] Initializing Storage Manager at: " << db_path << std::endl;
    auto storage_manager = std::make_shared<titiansearch::storage::StorageManager>(db_path);
    if (!storage_manager->open()) {
        std::cerr << "Error: Failed to open database." << std::endl;
        return;
    }
    std::cout << "      Storage Manager initialized.\n\n";

    // 2. 初始化索引器
    // Indexer 负责将文档数据写入到 StorageManager
    std::cout << "[2/4] Initializing Indexer..." << std::endl;
    auto indexer = std::make_shared<titiansearch::indexing::Indexer>(storage_manager);
    std::cout << "      Indexer initialized.\n\n";

    // 3. 初始化爬虫
    // Crawler 负责抓取网页，并将解析后的内容传递给 Indexer
    std::cout << "[3/4] Initializing Crawler..." << std::endl;
    titiansearch::crawler::CrawlerConfig crawler_config;
    crawler_config.max_pages_to_crawl = 100; // 示例配置
    crawler_config.num_threads = 4;
    titiansearch::crawler::Crawler crawler(crawler_config, indexer);
    std::cout << "      Crawler initialized.\n\n";

    // 4. 启动爬虫
    std::cout << "[4/4] Starting crawl process with seed URLs..." << std::endl;
    crawler.start(seed_urls);
    std::cout << "\nCrawl and index process finished.\n";
    std::cout << "Total documents in index: " << storage_manager->get_total_docs_count() << std::endl;
}

// 交互式查询模式
void run_interactive_query_mode() {
    std::cout << "=======================================\n";
    std::cout << " TitanSearch: Interactive Query Mode \n";
    std::cout << "=======================================\n\n";

    std::string db_path = "./data/index";

    // 1. 初始化存储管理器 (只读模式)
    std::cout << "[1/2] Initializing Storage Manager (Read-Only)..." << std::endl;
    auto storage_manager = std::make_shared<titiansearch::storage::StorageManager>(db_path);
    if (!storage_manager->open(true)) { // true for read-only
        std::cerr << "Error: Failed to open database. Did you run the crawl mode first?" << std::endl;
        return;
    }
    std::cout << "      Storage Manager initialized.\n\n";

    // 2. 初始化查询处理器
    std::cout << "[2/2] Initializing Query Processor..." << std::endl;
    titiansearch::querying::QueryProcessor query_processor(storage_manager);
    std::cout << "      Query Processor initialized.\n\n";

    std::cout << "Welcome to TitanSearch! Type 'exit' to quit." << std::endl;
    std::string query;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, query);

        if (query == "exit") {
            break;
        }

        if (query.empty()) {
            continue;
        }

        // 3. 执行查询并打印结果
        auto start_time = titiansearch::common::get_current_time_ms();
        std::vector<titiansearch::querying::SearchResult> results = query_processor.search(query);
        auto end_time = titiansearch::common::get_current_time_ms();

        std::cout << "\nFound " << results.size() << " results in " << (end_time - start_time) << " ms.\n";
        std::cout << "--------------------------------------------------\n";

        int rank = 1;
        for (const auto& res : results) {
            std::cout << rank++ << ". " << res.title << " (Score: " << res.score << ")\n";
            std::cout << "   " << res.url << "\n";
            std::cout << "   ... " << res.snippet << " ...\n\n";
        }
    }
}

int main(int argc, char* argv[]) {
    // 设置程序使用的区域为UTF-8，以正确处理中文
    // 在Windows上可能需要更复杂的设置，但这是一个好的开始
    try {
        std::locale::global(std::locale("en_US.UTF-8"));
    } catch (const std::runtime_error& e) {
        // 如果系统不支持，则 gracefully fail
        std::cerr << "Warning: Could not set locale to UTF-8. " << e.what() << std::endl;
    }
    std::cout.imbue(std::locale());
    std::cin.imbue(std::locale());
    
    std::cout << "|| TitanSearch Engine v0.1 (Placeholder) ||\n\n";

    // 简单的模式切换
    std::string mode;
    parse_arguments(argc, argv, mode);

    if (mode == "crawl") {
        run_crawl_and_index_mode();
    } else if (mode == "interactive") {
        run_interactive_query_mode();
    } else {
        std::cerr << "Usage: " << argv[0] << " [mode]\n";
        std::cerr << "Modes:\n";
        std::cerr << "  crawl        - Start crawling websites and building the index.\n";
        std::cerr << "  interactive  - Start interactive query shell (default).\n";
        return 1;
    }

    return 0;
}