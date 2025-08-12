
#include "syncConPool.h"


/**
* @brief 初始化连接池
*
* 初始化连接池，并设置MySQL连接信息。
*
* @param info MySQL连接信息结构体
*/
void ConnectPool::initialize(const mysqlinfo &info)
{
    std::lock_guard<std::mutex> lock(m_mtx);

    if(!m_isInit){
        m_sqlInfo.host = info.host;
        m_sqlInfo.user = info.user;
        m_sqlInfo.passwd = info.passwd;
        m_sqlInfo.dbname = info.dbname;
        m_sqlInfo.port = info.port;
        m_sqlInfo.max_connections = info.max_connections;

        try{
            m_driver = sql::mysql::get_driver_instance();
            for(int i=0;i<info.max_connections;++i){
                createConnection();
            }
            m_isInit = true;
            std::cout << "Connection pool initialized successfully." << std::endl;
        }catch(sql::SQLException &e){
            std::cerr << "Failed to initialize Mysql: " << e.what() << std::endl;
            std::cerr<<"Error Code: "<<e.getErrorCode()<<std::endl;
        }
    }
}


/**
* @brief 获取数据库连接
*
* 获取一个指向 sql::Connection 对象的 shared_ptr 智能指针。
*
* @return std::shared_ptr<sql::Connection> 指向 sql::Connection 对象的智能指针
*/
std::shared_ptr<sql::Connection> ConnectPool::getConnection()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    // 等待直到有可用连接
    if(m_freeConnPool.empty()){
        m_condition.wait(lock, [this](){ return !m_freeConnPool.empty(); });
    }
    auto conn = m_freeConnPool.front();
    m_freeConnPool.pop();

    // 检查链接是否有效
    if(!conn->isValid() || conn->isClosed()){
        try{
            conn->reconnect();
            std::cout<<"Reconnecting..."<<std::endl;
        }catch(sql::SQLException &e){
            std::cerr<<"Failed to reconnect: "<<e.what()<<std::endl;
            conn = createConnection();
        }
    }
    return conn;
}

/**
* @brief 归还数据库连接到连接池
*
* 将使用完毕的数据库连接对象安全地返回到连接池中。函数会检查连接有效性，
* 若连接已关闭则会创建新连接替代，保证连接池的健康状态。
*
* @param conn 指向sql::Connection对象的共享指针，表示要归还的数据库连接
*
* @note 使用互斥锁保证线程安全，归还有效连接时会唤醒等待连接的线程
*/
void ConnectPool::returnConnection(std::shared_ptr<sql::Connection> conn)
{
    std::lock_guard<std::mutex> lock(m_mtx);

    // 检查连接是否有效
    if(conn->isValid() && !conn->isClosed()){
        m_freeConnPool.push(conn);
        m_condition.notify_one();
    }else{
        // 如果连接已关闭，创建新的连接代替
        std::cout<<"Connection is closed, creating a new one...\n";
        createConnection();
    }
}

/**
* @brief 打印连接池信息
*
* 该函数会打印当前连接池的基本信息，包括主机名、用户名、数据库名、端口号、总连接数、可用连接数和已使用连接数。
* 在打印信息前，会对互斥锁进行加锁，以保证线程安全。
*/
void ConnectPool::printInfo()const
{
    std::lock_guard<std::mutex> lock(m_mtx);

    std::cout << "Connection Pool Info:" << std::endl;
    std::cout << "Host: " << m_sqlInfo.host << std::endl;
    std::cout << "User: " << m_sqlInfo.user << std::endl;
    std::cout << "Database Name: " << m_sqlInfo.dbname << std::endl;
    std::cout << "Port: " << m_sqlInfo.port << std::endl;
    std::cout << "Total connections: "<<m_sqlInfo.max_connections<<std::endl;
    std::cout << "Available connections: "<<m_freeConnPool.size()<<std::endl;
    std::cout << "Used connections: "<<m_sqlInfo.max_connections - m_freeConnPool.size()<<std::endl;
}


/**
* @brief 创建数据库连接
*
* 使用提供的数据库驱动和连接信息创建数据库连接，并将其加入空闲连接池。
*
* @return 成功时返回连接对象的共享指针，失败时返回nullptr。
*/
std::shared_ptr<sql::Connection> ConnectPool::createConnection()
{
    try{
        auto conn = std::shared_ptr<sql::Connection>(
            m_driver->connect(m_sqlInfo.host, m_sqlInfo.user, m_sqlInfo.passwd),
            [this](sql::Connection* conn){
                returnConnection(std::shared_ptr<sql::Connection>(conn));
            }
        );

        conn->setSchema(m_sqlInfo.dbname);
        m_freeConnPool.push(conn);
        return conn;
    }catch(sql::SQLException &e){
        std::cerr<<"Failed to create connection: "<<e.what()<<std::endl;
        return nullptr;
    }
}