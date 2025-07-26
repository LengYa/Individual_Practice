#include <iostream>
#include <unordered_map>
#include <list>

using namespace std;

template <typename K, typename V>
class LFUCache
{
public:
    struct Node {
        K key;
        V value;
        int freq;       // 频率
        Node(K key, V val, int f) :key(key), value(val), freq(f) {}
    };

    LFUCache(size_t capacity) :m_capacity(capacity), m_minFreq(0) {}

    V get(K key)
    {
        if (m_cache.find(key) == m_cache.end()) throw out_of_range("Key not found");

        // 获取节点，并更新频率
        auto node = m_cache[key];
        updateFreq(node);
        
        // 重新使迭代器指向key
        node = m_cache[key];
        return node->value;
    }

    void put(K key, V value)
    {
        if (m_capacity <= 0) return;

        // 如果键已存在, 更新值和频率
        if (m_cache.find(key) != m_cache.end()) {
            auto node = m_cache[key];
            node->value = value;
            updateFreq(node);
            return;
        }


        // 如果缓存已满，则移除最少使用的节点
        if (m_cache.size() >= m_capacity)
        {
            // 移除最少使用的节点
            auto& minFreqList = m_freqList[m_minFreq];
            auto del_node = minFreqList.back();
            m_cache.erase(del_node.key);
            minFreqList.pop_back();

            // 如果链表为空，清除频率
            if (minFreqList.empty()) {
                m_freqList.erase(m_minFreq);
                if (m_minFreq > 1) --m_minFreq; // 更新最小频率值
            }
        }

        // 插入新节点,频率为1
        m_minFreq = 1; // 新节点频率为1，更新最小频率值
        m_freqList[m_minFreq].emplace_front(key, value, m_minFreq);
        m_cache[key] = m_freqList[1].begin();
    }
    void print()
    {
        cout << "==============================\n";
        cout << "Print\n";
        for (auto& val : m_freqList) {
            cout << "Frequency: " << val.first;
            for (auto& node : val.second) {
                cout << "  Key: " << node.key << " value: " << node.value << endl;
            }
        }
        cout << "==============================\n";
    }
private:
    size_t m_capacity;
    int m_minFreq;      // 记录当前最小的频率
    unordered_map<K, typename list<Node>::iterator> m_cache;
    unordered_map<int, list<Node>> m_freqList; // 频率列表，key为频率值，value为该频率下所有节点构成的链表

    void updateFreq(typename list<Node>::iterator node)
    {
        // 保存节点的键和值
        K key = node->key;
        V value = node->value;

        // 从原频率链表移除
        int old_freq = node->freq;
        auto& old_list = m_freqList[old_freq];
        old_list.erase(node);

        // 如果频率链表为空，则更新最小频率值
        if (old_list.empty()) {
            m_freqList.erase(old_freq); // 移除空链表
            if (old_freq == m_minFreq)
                ++m_minFreq;
        }

        // 插入到新频率链表中
        int new_freq = old_freq + 1;
        m_freqList[new_freq].emplace_front(key, value, new_freq);
        m_cache[key] = m_freqList[new_freq].begin();
    }
};


int main()
{
    LFUCache<int, int> cache(3);

    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);

    cache.print();

    cout << "Get key 2: " << cache.get(2) << endl;
    cache.print();
    cache.put(4, 4);
    cache.print();
    try {
        cout << "Get key 1: " << cache.get(1) << endl; // (not found)
    }
    catch (out_of_range& e) {
        cout << "get key 1: " << e.what() << endl;
    }
    cache.print();
    cout << "Get key 3: " << cache.get(3) << endl; // 3
    cout << "Get key 4: " << cache.get(4) << endl; // 4
    cache.print();

    return 0;
}
