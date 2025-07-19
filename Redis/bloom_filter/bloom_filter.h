#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <functional>
#include <hiredis/hiredis.h>

class BloomFilter {
public:
    BloomFilter(const std::string& redis_host, int redis_port,
                const std::string& key, 
                const std::string& password = "",
                size_t expected_items = 10000, 
                double false_positive_rate = 0.01);

    ~BloomFilter();

    // 连接 Redis
    void connect_redis(const std::string& redis_host, int redis_port);

    // 向布隆过滤器中添加元素
    void add(const std::string& element);

    // 检查元素是否可能存在于布隆过滤器中
    bool searchKey(const std::string& element);

    // 获取布隆过滤器的统计信息
    void print_stats() const;

private:
    // 计算最优的位图大小和哈希函数数量
    void calculate_optimal_parameters(size_t n, double p);

    // 初始化哈希函数
    void initialize_hash_functions();

    // 计算位位置
    size_t calculate_bit_position(const std::string& element, size_t hash_index) const;

    redisContext* m_redis_conn;             // Redis 连接
    std::string m_redis_key;                // Redis 键名
    const std::string m_redis_password;     // Redis 密码

    size_t m_bitmap_size;                   // 位图大小（位数）
    size_t m_num_hashes;                    // 哈希函数数量
    double m_false_positive_rate;           // 误判率
    
    std::vector<std::function<size_t(const std::string&)>> m_hash_functions;
};