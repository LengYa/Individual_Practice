#include <iostream>
#include <list>
#include <unordered_map>

using namespace std;

template<typename K, typename V>
class LRUCache
{
    public:
        LRUCache(size_t capacity) : m_capacity(capacity){}

        V get(K key)
        {
            auto it = m_cache.find(key);
            if (it == m_cache.end()) throw out_of_range("Key not found");

            m_lru.splice(m_lru.begin(), m_lru, it->second); // 将节点移动到链表头部
            return it->second->second;
        }

        void put(K key, V value)
        {
            auto it = m_cache.find(key);
            if (it != m_cache.end())
            {
                it->second->second = value; // 更新值
                m_lru.splice(m_lru.begin(), m_lru, it->second); // 更新节点位置
                return;
            }
            
            // 检查容量是否超出，如果超出则删除最旧的元素（尾部）
            if (m_lru.size() >= m_capacity)
            {
                // 淘汰尾部元素
                auto last = m_lru.back();
                m_cache.erase(last->first); // 从哈希表中删除该元素
                m_lru.pop_back(); // 移除链表中的最后一个节点
            }

            // 插入新元素到头部
            m_lru.emplace_front(key, value);
            m_cache[key] = m_lru.begin();
        }

        void print()
        {
            for (auto& item : m_lru)
                cout << "Key: " << item.first << ", Value: " << item.second << "-> ";

            cout<<"nullptr\n";
        }
    private:
        size_t m_capacity;
        list<pair<K, V>> m_lru;      //存储键值对，头部最新，尾部最旧
        unordered_map<K, typename list<pair<K, V>>::iterator> m_cache;
};

int main()
{
    LRUCache<int, int> cache(3);

    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    
    cache.print();
    cout<<"Get key 2: "<<cache.get(2)<<endl;

    cache.put(4, 4); // 淘汰键1，因为它是最旧的元素
    try{
        cout<<"Get key 1: "<<cache.get(1)<<endl; // 应该返回-1，因为键1已经被淘汰了
    }catch(out_of_range& e){
        cout << e.what() << endl;
    }

    cache.print();
    cout<<"Get key 3: "<<cache.get(3)<<endl;
    cache.print();

    return 0;
}