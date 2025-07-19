#include "bloom_filter.h"

using namespace std;

/**
* @brief 构造函数
*
* @param redis_host Redis 主机地址
* @param redis_port Redis 端口
* @param password Redis 密码
* @param key Redis 中存储位图的键名
* @param expected_items 预期存储的元素数量
* @param m_false_positive_rate 可接受的误判率 (0.01 表示 1%)
*/
BloomFilter::BloomFilter(const std::string& redis_host, int redis_port,
	const std::string& key,
    const std::string& password,
	size_t expected_items,
	double m_false_positive_rate) :m_redis_password(password), m_redis_key(key), m_redis_conn(nullptr)
{
	//连接redis
    connect_redis(redis_host, redis_port);

    // 计算最优参数
	calculate_optimal_parameters(expected_items, m_false_positive_rate);

    // 初始化哈希函数
	initialize_hash_functions();
}

BloomFilter::~BloomFilter()
{
	if (m_redis_conn) {
		redisFree(m_redis_conn);
	}
}

/**
 * @brief 连接 Redis
 *
 * @param element 要添加的元素
 */
void BloomFilter::connect_redis(const string& redis_host, int redis_port)
{
    // 定义连接超时时间
    struct timeval timeout = { 1, 500000 }; // 1.5 秒

    // 初始化连接选项
    redisOptions options;
    memset(&options, 0, sizeof(redisOptions));

    // 设置 Redis 服务器的主机名和端口
    REDIS_OPTIONS_SET_TCP(&options, redis_host.c_str(), redis_port);

    // 设置连接超时和命令超时
    options.connect_timeout = &timeout;
    options.command_timeout = &timeout;

    // 尝试建立连接
    m_redis_conn = redisConnectWithOptions(&options);
    if (m_redis_conn == nullptr || m_redis_conn->err) {
        if (m_redis_conn) {
            cerr<<"Connection error: "<< m_redis_conn->errstr<<endl;
            redisFree(m_redis_conn);
        } else {
            cerr<<"Connection error: can not allocate redis context\n";
        }
        exit(1);
    }

    //认证密码，如果有密码的话，需要先认证一下，没有密码那就注释掉这行代码。
	redisReply *reply = reinterpret_cast<redisReply*>(redisCommand(
		m_redis_conn, "AUTH %s", m_redis_password.c_str()));    
	if (reply->type == REDIS_REPLY_ERROR) {
        cerr<<"Redis认证失败！\n";
        redisFree(m_redis_conn);
        exit(1);
    }
    else{
        cout<<"Redis认证成功！\n";
    }
}

/**
 * @brief 向布隆过滤器中添加元素
 *
 * @param element 要添加的元素
 */
void BloomFilter::add(const string& element) 
{
	for (size_t i = 0; i < m_num_hashes; ++i) {
		size_t bit_position = calculate_bit_position(element, i);
		redisCommand(m_redis_conn, "SETBIT %s %d 1", m_redis_key.c_str(), static_cast<int>(bit_position));
	}
}

/**
 * @brief 检查元素是否可能存在于布隆过滤器中
 *
 * @param element 要检查的元素
 * @return true 元素可能存在（可能有误判）
 * @return false 元素绝对不存在
 */
bool BloomFilter::searchKey(const std::string& element)
{
	for (size_t i = 0; i < m_num_hashes; ++i) {
		size_t bit_position = calculate_bit_position(element, i);

		redisReply* reply = static_cast<redisReply*>(
			redisCommand(m_redis_conn, "GETBIT %s %d", m_redis_key.c_str(), static_cast<int>(bit_position)));

		if (reply == nullptr) {
			std::cerr << "Redis command failed" << std::endl;
			continue;
		}

		// 如果任何一个位为0，则元素一定不存在
		if (reply->integer == 0) {
			freeReplyObject(reply);
			return false;
		}

		freeReplyObject(reply);
	}

	// 所有位都为1，元素可能存在
	return true;
}

/**
 * @brief 获取布隆过滤器的统计信息
 */
void BloomFilter::print_stats() const 
{
	std::cout << "Bloom Filter Statistics:\n";
	std::cout << "  Bitmap size: " << m_bitmap_size << " bits ("
		<< (m_bitmap_size / 8 / 1024.0) << " KB)\n";
	std::cout << "  Number of hash functions: " << m_num_hashes << "\n";
	std::cout << "  Expected false positive rate: "
		<< (m_false_positive_rate * 100) << "%\n";
}

/**
 * @brief 计算最优的位图大小和哈希函数数量
 */
void BloomFilter::calculate_optimal_parameters(size_t n, double p) 
{
	// 计算位图大小 m = - (n * ln(p)) / (ln(2)^2)
	m_bitmap_size = static_cast<size_t>(-(n * log(p)) / (log(2) * log(2)));

	// 确保位图大小至少为1
	if (m_bitmap_size == 0) m_bitmap_size = 1;

	// 计算哈希函数数量 k = (m / n) * ln(2)
	m_num_hashes = static_cast<size_t>(ceil((m_bitmap_size / static_cast<double>(n)) * log(2)));

	// 至少需要1个哈希函数
	if (m_num_hashes == 0) m_num_hashes = 1;

	m_false_positive_rate = p;
}

/**
 * @brief 初始化哈希函数
 */
void BloomFilter::initialize_hash_functions() 
{
	// 使用不同的种子创建多个哈希函数
	for (size_t i = 0; i < m_num_hashes; ++i) {
		m_hash_functions.emplace_back([seed = i](const string& str) -> size_t {
			// 简单的哈希函数实现 (FNV-1a算法)
			const size_t prime = 0x100000001b3;
			size_t hash = 0xcbf29ce484222325 ^ seed;

			for (char c : str) {
				hash ^= static_cast<size_t>(c);
				hash *= prime;
			}

			return hash;
		});
	}
}

/**
 * @brief 计算位位置
 */
size_t BloomFilter::calculate_bit_position(const std::string& element, size_t hash_index) const 
{
		size_t hash_value = m_hash_functions[hash_index](element);
		return hash_value % m_bitmap_size;
}