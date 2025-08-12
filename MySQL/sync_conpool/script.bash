#!/bin/bash
PROJECT_ROOT="./"  # 替换为您的项目根目录路径
BUILD_DIR="$PROJECT_ROOT/build"
BIN_DIR="../bin"

# 检查构建目录是否存在，如果不存在则创建
if [ ! -d "$BUILD_DIR" ]; then
  echo "构建目录不存在，正在创建..."
  mkdir -p "$BUILD_DIR"
  echo "构建目录创建成功。"
else
  echo "构建目录已存在,清空重新创建."
  rm -rf "$BUILD_DIR"/*
  mkdir -p "$BUILD_DIR"
  echo "构建目录创建成功。"
fi

# 检查二进制目录是否存在，如果不存在则创建
if [ ! -d "$BIN_DIR" ]; then
  echo "构建bin目录不存在，正在创建..."
  mkdir -p "$BIN_DIR"
  echo "构建bin目录创建成功。"
else
  echo "构建bin目录已存在"
fi

# 进入构建目录
cd "$BUILD_DIR" || { echo "无法进入构建目录"; exit 1; }

# 执行make clean（如果存在Makefile）
if [ -f Makefile ]; then
  echo "正在执行make clean..."
  make clean
  if [ $? -ne 0 ]; then
    echo "make clean失败，但继续执行。"
  fi
fi

# 执行cmake配置
echo "正在运行cmake配置..."
cmake ..
if [ $? -ne 0 ]; then
  echo "cmake配置失败。"
  exit 1
fi
echo "cmake配置成功。"


# 执行make install
echo "正在执行make install..."
make install
if [ $? -ne 0 ]; then
  echo "make install失败。"
  exit 1
fi
echo "make install成功。"

echo "所有操作完成。"