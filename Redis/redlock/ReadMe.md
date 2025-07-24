# 设置环境变量
```bash
path=""
export LD_LIBRARY_PATH=/$path/redis-plus-plus/build::$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/$path/redis-7.4.0/deps/hiredis/build:$LD_LIBRARY_PATH
```
# 编译指令
```bash
g++ -std=c++17 main.cpp -lredis++ -lhiredis -pthread -g -o exe
```