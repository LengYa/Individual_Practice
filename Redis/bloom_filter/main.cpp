#include <random>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <ctime>

#include "bloom_filter.h"

using namespace std;

vector<string> generate_email_accounts(size_t count);
vector<string> select_blacklist(const vector<string>& emails, size_t blacklist_size,
    const string& filename = "blacklist.txt");

void simulate_email_processing(BloomFilter& bloom_filter,
    const vector<string>& all_emails,
    const vector<string>& blacklist,
    const string& stats_filename = "email_stats.csv");

int main() 
{
    const string hostname = "127.0.0.1";
    const int port = 6379;
    const string password = "123456";
    try {
        cout << "===== Email Blacklist Simulation =====\n\n";
        
        // 步骤1: 生成10万个邮箱账号
        cout << "Generating 100,000 email accounts...\n";
        auto start_time = time(nullptr);
        vector<string> emails = generate_email_accounts(100000);
        cout << "Generated " << emails.size() << " email accounts in " 
                  << (time(nullptr) - start_time) << " seconds.\n";
        
        // 步骤2: 从中选择5000个作为黑名单
        cout << "\nSelecting 5,000 blacklisted emails...\n";
        start_time = time(nullptr);
        vector<string> blacklist = select_blacklist(emails, 5000);
        cout << "Selected " << blacklist.size() << " blacklisted emails in " 
                  << (time(nullptr) - start_time) << " seconds.\n";
        
        // 步骤3: 初始化布隆过滤器
        BloomFilter bloom_filter("127.0.0.1", port, "my_bloom_filter", password, 100000, 0.01);

        // 打印统计信息
        bloom_filter.print_stats();
        
        // 步骤4: 模拟邮件处理过程
        cout << "\nSimulating email processing...\n";
        start_time = time(nullptr);
        simulate_email_processing(bloom_filter, emails, blacklist);
        cout << "Processing completed in " << (time(nullptr) - start_time) << " seconds.\n";

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

// 生成随机邮件账号
vector<string> generate_email_accounts(size_t count) 
{
    vector<string> emails;
    emails.reserve(count);
    
    // 常见邮箱域名
    const vector<string> domains = {
        "gmail.com", "yahoo.com", "hotmail.com", "outlook.com",
        "icloud.com", "protonmail.com", "aol.com", "zoho.com"
    };
    
    // 随机数生成器
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> domain_dist(0, domains.size() - 1);
    uniform_int_distribution<> num_dist(1000, 9999999);
    
    for (size_t i = 0; i < count; ++i) {
        // 随机组合邮箱地址
        string email = to_string(num_dist(gen)) + "@" + domains[domain_dist(gen)];
        emails.push_back(email);
    }
    
    return emails;
}

// 从邮箱列表中随机选择黑名单
vector<string> select_blacklist(const vector<string>& emails, 
    size_t blacklist_size,
    const string& filename) 
{
    if (blacklist_size > emails.size()) {
        throw invalid_argument("Blacklist size cannot exceed total emails");
    }
    
    // 复制邮箱列表
    vector<string> shuffled = emails;
    
    // 随机打乱
    random_device rd;
    mt19937 gen(rd());
    shuffle(shuffled.begin(), shuffled.end(), gen);
    
    // 选择前 blacklist_size 个作为黑名单
    vector<string> blacklist(
        shuffled.begin(), 
        shuffled.begin() + blacklist_size
    );
    
    // 保存黑名单到文件
    ofstream outfile(filename);
    if (outfile.is_open()) {
        for (const auto& email : blacklist) {
            outfile << email << "\n";
        }
        outfile.close();
        cout << "Saved blacklist to " << filename << "\n";
    } else {
        cerr << "Failed to save blacklist to file\n";
    }
    
    return blacklist;
}


// 模拟邮件处理流程
void simulate_email_processing(
    BloomFilter& bloom_filter,
    const vector<string>& all_emails,
    const vector<string>& blacklist,
    const string& stats_filename) 
{
    // 将黑名单添加到布隆过滤器
    cout << "Adding " << blacklist.size() << " blacklisted emails to Bloom filter...\n";
    for (const auto& email : blacklist) {
        bloom_filter.add(email);
    }
    
    // 打开统计文件
    ofstream stats_file(stats_filename);
    if (!stats_file.is_open()) {
        cerr << "Failed to open stats file: " << stats_filename << "\n";
        return;
    }
    
    // 写入CSV头部
    stats_file << "Email,IsBlacklisted,DetectedAsBlacklisted,IsFalsePositive\n";
    
    // 统计变量
    size_t total_emails = all_emails.size();
    size_t actual_blacklisted = blacklist.size();
    size_t detected_blacklisted = 0;
    size_t false_positives = 0;
    size_t false_negatives = 0;
    
    // 处理每个邮件
    for (const auto& email : all_emails) {
        // 检查是否在黑名单中（实际）
        bool is_blacklisted = find(blacklist.begin(), blacklist.end(), email) != blacklist.end();
        
        // 使用布隆过滤器检测
        bool detected = bloom_filter.searchKey(email);
        
        // 计算是否误判
        bool is_false_positive = !is_blacklisted && detected;
        
        // 更新统计
        if (is_blacklisted) {
            if (detected) detected_blacklisted++;
            else false_negatives++;
        } else {
            if (detected) false_positives++;
        }
        
        // 写入统计文件
        stats_file << email << ","
                   << (is_blacklisted ? "true" : "false") << ","
                   << (detected ? "true" : "false") << ","
                   << (is_false_positive ? "true" : "false") << "\n";
    }
    
    stats_file.close();
    
    // 计算统计指标
    double false_positive_rate = static_cast<double>(false_positives) / 
                                (total_emails - actual_blacklisted);
    double detection_rate = static_cast<double>(detected_blacklisted) / actual_blacklisted;
    
    // 打印统计结果
    cout << "\n===== Email Processing Results =====\n";
    cout << "Total emails processed: " << total_emails << "\n";
    cout << "Actual blacklisted emails: " << actual_blacklisted << "\n";
    cout << "Detected blacklisted emails: " << detected_blacklisted << "\n";
    cout << "False negatives: " << false_negatives << "\n";
    cout << "False positives: " << false_positives << "\n";
    cout << "False positive rate: " << (false_positive_rate * 100) << "%\n";
    cout << "Detection rate: " << (detection_rate * 100) << "%\n";
    cout << "Saved detailed stats to: " << stats_filename << "\n";
}
