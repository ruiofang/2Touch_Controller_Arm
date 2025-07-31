#!/bin/bash

# 双触觉设备双机械臂控制系统启动脚本
# 仅过滤 hEvent is NULL 调试信息，保留其他所有日志

echo "=== 双触觉设备双机械臂控制系统 ==="
echo "正在启动程序（过滤hEvent调试信息）..."
echo ""

# 启动程序并过滤 hEvent is NULL 信息
./Touch_Controller_Arm2 2>&1 | grep -v "hEvent is NULL"

echo ""
echo "程序已退出" 