# Individual_Practice
Store the code for personal practice

Pragmas:
# 设置环境遍历
export LD_LIBRARY_PATH=/opt/redis-7.4.0/deps/hiredis/build:$LD_LIBRARY_PATH

# 编译指令
g++ -std=c++17 bloom_filter.cpp main.cpp -lhiredis -g -o exe
