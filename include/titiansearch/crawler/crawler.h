#pragma once

#include <vector>
#include <string>
#include <memory>

#include "titansearch/indexing/indexer.h" // 依赖 indexer

namespace titiansearch {
namespace crawler {

struct CrawlerConfig {
    int max_pages_to_crawl = 1000;
    int num_threads = 4;
};

class Crawler {
public:
    Crawler(const CrawlerConfig& config, std::shared_ptr<indexing::Indexer> indexer);
    ~Crawler();

    void start(const std::vector<std::string>& seed_urls);

private:
    CrawlerConfig config_;
    std::shared_ptr<indexing::Indexer> indexer_;
    // TODO: Add members for URL frontier, thread pool, etc.
};

} // namespace crawler
} // namespace titiansearch