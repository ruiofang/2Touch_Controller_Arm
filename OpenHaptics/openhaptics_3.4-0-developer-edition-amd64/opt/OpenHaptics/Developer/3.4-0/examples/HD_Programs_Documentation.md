# OpenHaptics HD 示例程序文档

## 概述

本文档详细描述了 OpenHaptics 工具包中 HD 目录下的所有示例程序的功能、特性以及编译所需的依赖库。这些程序演示了如何使用 OpenHaptics Haptic Device (HD) API 进行底层触觉设备控制和力反馈渲染。

HD API 是 OpenHaptics 的底层接口，提供对触觉设备的直接控制，包括位置读取、力输出、设备状态管理等核心功能。

## 目录结构

```
HD/
├── console/          # 控制台程序（无图形界面）
└── graphics/         # 图形程序（包含OpenGL渲染）
```

## 控制台程序 (console/)

### 1. HelloHapticDevice
**功能描述：** HD API 的入门示例程序，演示重力井效果。
- 创建一个重力井力场，当设备接近中心时产生吸引力
- 演示基本的 HD API 初始化和设备控制
- 使用调度器（scheduler）处理力反馈回调
- 展示最基本的力计算和输出

**主要特性：**
- 基础的 HDAPI 初始化 (`hdInitDevice`)
- 异步调度器回调 (`hdScheduleAsynchronous`)
- 重力井物理模拟（基于胡克定律 F=kx）
- 实时力反馈输出
- 键盘输入检测退出

**物理模型：**
- 重力井影响范围：40mm
- 弹性系数：0.075 N/mm
- 力的方向：设备位置指向井中心
- 使用弹簧模型而非真实重力模型

### 2. FrictionlessSphere
**功能描述：** 无摩擦球体碰撞检测和力反馈演示。
- 模拟一个位于原点、半径40mm的无摩擦球体
- 当设备穿透球体时产生排斥力
- 演示碰撞检测和表面法向力计算

**主要特性：**
- 球体几何碰撞检测
- 距离计算和穿透深度分析
- 表面法向力计算
- 实时球面约束

**物理参数：**
- 球体半径：40mm
- 球心位置：(0,0,0)
- 表面刚度：0.25 N/mm
- 力方向：从球心指向设备位置

### 3. FrictionlessPlane
**功能描述：** 无摩擦平面约束演示。
- 模拟一个无摩擦的约束平面
- 防止设备穿透平面
- 演示平面碰撞检测和法向力

### 4. AnchoredSpringForce
**功能描述：** 锚定弹簧力演示。
- 在设备当前位置创建一个虚拟锚点
- 模拟弹簧连接设备和锚点
- 演示弹性恢复力和阻尼

**主要特性：**
- 动态锚点设置
- 弹簧力计算 (F = -k * displacement)
- 可调节弹性系数
- 实时位置跟踪

### 5. QueryDevice
**功能描述：** 设备信息查询工具。
- 查询和显示触觉设备的详细信息
- 获取设备能力和规格参数
- 显示工作空间尺寸和分辨率

**查询信息包括：**
- 设备型号和序列号
- 工作空间尺寸
- 分辨率和精度
- 最大输出力
- 设备校准状态

### 6. Calibration
**功能描述：** 设备校准程序。
- 执行设备校准程序
- 检测和更新校准状态
- 确保设备精度和可靠性

### 7. ErrorHandling
**功能描述：** 错误处理机制演示。
- 演示各种 HD API 错误的检测和处理
- 展示错误恢复策略
- 错误信息的格式化输出

**错误类型：**
- 设备初始化错误
- 调度器错误
- 通信错误
- 校准错误

### 8. PreventWarmMotors
**功能描述：** 电机过热保护演示。
- 监控电机温度状态
- 实现过热保护机制
- 演示安全的力输出策略

### 9. ServoLoopRate
**功能描述：** 伺服循环频率监控和调整。
- 监控触觉伺服循环的实际频率
- 分析性能统计数据
- 优化系统性能

**功能特性：**
- 实时频率监控
- 统计数据收集
- 性能分析工具
- 频率稳定性测试

### 10. ServoLoopDutyCycle
**功能描述：** 伺服循环占空比分析。
- 测量和分析伺服循环的占空比
- 监控系统负载
- 优化实时性能

### 11. CommandJointTorque
**功能描述：** 关节力矩直接控制。
- 直接控制设备各关节的力矩输出
- 演示底层电机控制
- 关节空间力控制

**控制特性：**
- 独立关节控制
- 力矩限制和安全
- 实时力矩监控
- 关节角度反馈

### 12. CommandMotorDAC
**功能描述：** 电机 DAC 直接控制（高级功能）。
- 直接控制电机的 DAC 输出值
- 最底层的设备控制
- 用于高级调试和开发

**警告：** 这是非常底层的控制，使用不当可能损坏设备。

### 13. Vibration
**功能描述：** 振动效果生成器。
- 生成各种频率和幅度的振动
- 演示周期性力输出
- 触觉纹理和振动反馈

**振动参数：**
- 可调频率范围
- 可调振幅
- 多种波形支持
- 实时参数调整

## 图形程序 (graphics/)

### 1. SimpleHapticScene
**功能描述：** 基础图形+触觉场景演示。
- 展示图形渲染与触觉反馈的集成
- 3D 光标显示和工作空间映射
- 触觉锚点和约束演示

**主要特性：**
- OpenGL 3D 渲染
- 触觉设备工作空间映射
- 实时 3D 光标显示
- 视角变换和投影设置
- 触觉状态与图形同步

**技术实现：**
- 双线程架构（图形线程+触觉线程）
- 工作空间到视图空间的变换
- 触觉状态的线程安全访问
- OpenGL 显示列表优化

### 2. PointManipulation
**功能描述：** 精确点操作和定位。
- 高精度点选择和操作
- 多点约束和引导
- 精细运动控制

### 3. PointSnapping
**功能描述：** 点吸附和对齐功能。
- 实现点到点、点到线、点到面的吸附
- 磁性吸附效果
- 辅助精确定位

### 4. SlidingContact
**功能描述：** 滑动接触和摩擦模拟。
- 模拟表面滑动摩擦
- 静摩擦和动摩擦切换
- 真实的表面触感

### 5. ParticleWaltz
**功能描述：** 粒子系统交互演示。
- 多粒子物理模拟
- 粒子间作用力
- 复杂动态系统

### 6. CoulombField
**功能描述：** 库仑力场模拟。
- 电场力模拟
- 多点电荷系统
- 力场可视化

### 7. CoulombForceDual
**功能描述：** 双设备库仑力演示。
- 支持两个触觉设备
- 设备间相互作用
- 协作操作演示

## 编译依赖库

### 核心依赖

#### OpenHaptics 库
```makefile
-lHD     # Haptic Device (底层设备API)
-lHDU    # Haptic Device Utilities
```

#### 图形程序额外依赖
```makefile
-lGL     # OpenGL 核心库
-lGLU    # OpenGL Utility Library
-lglut   # OpenGL Utility Toolkit
```

#### 系统库
```makefile
-lrt         # 实时扩展库 (POSIX)
-lncursesw   # 宽字符终端库
-lstdc++     # C++ 标准库
-lm          # 数学库
```

### 编译标志
```makefile
# 控制台程序
CFLAGS = -W -O2 -DNDEBUG -Dlinux

# C++ 程序
CXXFLAGS = -W -fexceptions -O2 -DNDEBUG -Dlinux

# 图形程序
CFLAGS += -lGL -lGLU -lglut
```

### 依赖问题解决

#### 1. ncurses 库依赖
HD 控制台程序需要 ncurses 库支持：
```bash
# Ubuntu/Debian
sudo apt-get install libncurses5-dev libncursesw5-dev

# CentOS/RHEL/Fedora
sudo yum install ncurses-devel
```

#### 2. 实时库依赖
某些程序需要 POSIX 实时扩展：
```bash
# 通常已包含在 glibc 中，无需额外安装
# 编译时需要链接 -lrt
```

#### 3. OpenGL 依赖（仅图形程序）
```bash
# Ubuntu/Debian
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev

# CentOS/RHEL/Fedora
sudo yum install mesa-libGL-devel mesa-libGLU-devel freeglut-devel
```

#### 4. 编译器要求
- GCC 4.8+ 或更高版本
- 支持 C++11 标准（部分程序）
- 64位系统推荐

## 性能和实时性要求

### 触觉循环频率
- **标准频率：** 1000 Hz (1ms 周期)
- **高性能：** 可达 4000 Hz
- **实时要求：** 硬实时，不能丢失周期

### 系统优化建议
1. **使用实时内核：** `PREEMPT_RT` 补丁
2. **CPU 亲和性：** 绑定触觉线程到专用核心
3. **内存锁定：** 防止触觉代码被换出
4. **中断优化：** 调整系统中断优先级

### 延迟要求
- **最大延迟：** < 1ms
- **抖动：** < 0.1ms
- **稳定性：** 99.9% 周期命中率

## 安全注意事项

### 力输出限制
```c
// 设置最大输出力（以牛顿为单位）
const double MAX_FORCE = 3.3;  // Phantom Omni 最大值
hdSetDoublev(HD_MAX_FORCE, &MAX_FORCE);
```

### 错误检测
```c
HDErrorInfo error;
if (HD_DEVICE_ERROR(error = hdGetError()))
{
    hduPrintError(stderr, &error, "设备错误");
    // 立即停止力输出
    hdDisableDevice(hHD);
}
```

### 安全关闭
```c
// 程序退出时的安全清理
void cleanup()
{
    hdStopScheduler();
    hdUnschedule(hCallback);
    hdDisableDevice(hHD);
}
```

## 使用说明

### 编译所有程序
```bash
cd HD/console
make

cd ../graphics
make
```

### 编译单个程序
```bash
cd HD/console/HelloHapticDevice
make
```

### 运行程序
```bash
# 确保设备已连接并有适当权限
./HelloHapticDevice
```

### 设备权限设置
```bash
# 添加用户到 dialout 组
sudo usermod -a -G dialout $USER

# 或设置 udev 规则
sudo nano /etc/udev/rules.d/99-phantom.rules
# 添加：SUBSYSTEM=="usb", ATTRS{idVendor}=="0403", MODE="0666"
```

## 故障排除

### 常见问题

#### 1. 设备未找到
```bash
# 检查设备连接
lsusb | grep -i phantom

# 检查权限
ls -l /dev/phantom*
```

#### 2. 实时性能问题
```bash
# 检查系统负载
htop

# 监控触觉循环
./ServoLoopRate
```

#### 3. 编译错误
```bash
# 检查 OpenHaptics 安装
echo $OPENHAPTICS_ROOT
ls -la $OPENHAPTICS_ROOT/lib*
```

#### 4. 运行时错误
- **Force too high：** 检查力计算和限制
- **Device communication error：** 检查 USB 连接
- **Scheduler error：** 检查实时性能

## 开发建议

### 1. 程序架构
- 使用异步调度器处理触觉循环
- 保持触觉回调函数简单高效
- 避免在触觉线程中进行复杂计算

### 2. 性能优化
- 预计算常用数学函数
- 使用内联函数减少调用开销
- 避免动态内存分配

### 3. 调试技巧
- 使用 `ServoLoopRate` 监控性能
- 记录力输出历史用于分析
- 使用示波器查看实际力输出

### 4. 安全编程
- 始终检查 HD API 返回值
- 实现力输出限制和监控
- 提供紧急停止机制

## 学习路径

### 初级（控制台程序）
1. **HelloHapticDevice** - 基础概念
2. **QueryDevice** - 设备信息
3. **FrictionlessSphere** - 简单几何体
4. **AnchoredSpringForce** - 弹性力

### 中级（错误处理和优化）
1. **ErrorHandling** - 错误处理
2. **ServoLoopRate** - 性能监控
3. **Calibration** - 设备校准
4. **PreventWarmMotors** - 安全机制

### 高级（图形集成和复杂交互）
1. **SimpleHapticScene** - 图形集成
2. **PointManipulation** - 精确控制
3. **ParticleWaltz** - 复杂系统
4. **CoulombForceDual** - 多设备

---

**注意：** 本文档基于 OpenHaptics 3.4.0 版本编写。HD API 是底层接口，使用时需要特别注意安全性和实时性要求。不当使用可能导致设备损坏或用户受伤。 