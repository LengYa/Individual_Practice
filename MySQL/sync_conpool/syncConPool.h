#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

#include <mysql_driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>

typedef struct st_mysqlinfo
{
    std::string host;
    std::string user;
    std::string passwd;
    std::string dbname;
    unsigned int port;
    unsigned int max_connections;

    st_mysqlinfo(){}
    st_mysqlinfo(std::string host, std::string user, std::string passwd,
                  std::string dbname, unsigned int port = 3306,
                  unsigned int max_connections = 5)
    {
        this->host = host;
        this->user = user;
        this->passwd = passwd;
        this->dbname = dbname;
        this->port = port;
        this->max_connections = max_connections;
    }
}mysqlinfo;

class ConnectPool
{
    public:
        static ConnectPool* getInstance(){
            static ConnectPool m_instance;

            return &m_instance;
        }

        void initialize(const mysqlinfo &info);

        std::shared_ptr<sql::Connection> getConnection();

        void returnConnection(std::shared_ptr<sql::Connection> conn);

        void printInfo()const;

    private:
        ConnectPool()
        {
            m_driver = nullptr;
            m_isInit = false;
        }
        ~ConnectPool(){
            while(!m_freeConnPool.empty()){
                auto conn = m_freeConnPool.front();
                m_freeConnPool.pop();
                if(!conn->isClosed()){
                    conn->close();
                }
            }
        }

        ConnectPool(const ConnectPool&) = delete;
        ConnectPool& operator=(const ConnectPool&) = delete;
        ConnectPool(ConnectPool&&) = delete;

        std::shared_ptr<sql::Connection> createConnection();

        sql::Driver* m_driver;
        std::queue<std::shared_ptr<sql::Connection>> m_freeConnPool;
        mutable std::mutex m_mtx;
        std::condition_variable m_condition;

        mysqlinfo m_sqlInfo;
        bool m_isInit;
};