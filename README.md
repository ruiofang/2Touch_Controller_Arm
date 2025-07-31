# 双Touch设备双机械臂控制系统

[![构建状态](https://img.shields.io/badge/构建-成功-brightgreen)]()
[![版本](https://img.shields.io/badge/版本-2.0.0-blue)]()
[![许可证](https://img.shields.io/badge/许可证-MIT-green)]()

## 🎯 项目概述

这是一个基于OpenHaptics的**双Touch设备双机械臂控制系统**，支持同时使用两个触觉设备分别控制两个机械臂，实现真正的双臂协同操作。

### ✨ 主要特性

- 🤖 **双机械臂控制**: 同时控制两个机械臂，Device1控制机械臂1，Device2控制机械臂2
- 👥 **双设备支持**: 支持两个Touch触觉设备独立操作，可独立配置映射参数
- 🛡️ **高度容错**: 机械臂连接失败时Touch设备仍可正常工作
- 🎮 **直观控制**: 按钮控制位置/姿态和夹抓操作
- 🔄 **实时反馈**: 高频率触觉力反馈 (1000Hz)
- 🌐 **TCP通信**: 基于JSON协议的机械臂通信
- ⚙️ **参数可调**: 实时调整映射系数和反馈强度
- 📝 **配置保存**: 自动保存用户偏好设置
- 🔧 **独立映射**: 每个设备可独立配置坐标映射关系和符号调整
- 🎛️ **设备识别**: 支持设备名称配置和自动识别

### 🚀 快速开始

#### 环境要求
- Linux系统 (Ubuntu 18.04+)
- OpenHaptics 3.4.0开发版
- CMake 3.10+ 或 Make
- 两个Touch触觉设备
- 两个支持TCP控制的机械臂

#### 1. 克隆项目
```bash
git clone <repository-url>
cd Touch_Controller_Arm2
```

#### 2. 检查环境
```bash
make check
```

#### 3. 编译运行
```bash
# 方式1: 使用CMake (推荐)
mkdir build && cd build
cmake .. && make -j$(nproc)
make run

# 方式2: 使用Makefile
make run

# 方式3: 使用启动脚本（过滤调试信息）
./run2.sh

# 方式4: 完整日志运行
make run-clean
```

## 📋 控制映射

### 设备控制
| 设备 | 按钮1 | 按钮2 |
|------|-------|-------|
| 设备1 | 机械臂1位置姿态 | 机械臂1夹抓 |
| 设备2 | 机械臂2位置姿态 | 机械臂2夹抓 |

### 默认坐标映射

#### 设备1映射（可配置）
| 触觉设备 | 机械臂1 | 符号 | 说明 |
|----------|---------|------|------|
| Z轴 | X轴 | 取反 | 触觉设备Z → 机械臂1 -X |
| X轴 | Y轴 | 取反 | 触觉设备X → 机械臂1 -Y |
| Y轴 | Z轴 | 正向 | 触觉设备Y → 机械臂1 Z |
| RZ轴 | RX轴 | 取反 | 触觉设备RZ → 机械臂1 -RX |
| RX轴 | RY轴 | 取反 | 触觉设备RX → 机械臂1 -RY |
| RY轴 | RZ轴 | 正向 | 触觉设备RY → 机械臂1 RZ |

#### 设备2映射（可配置）
| 触觉设备 | 机械臂2 | 符号 | 说明 |
|----------|---------|------|------|
| X轴 | X轴 | 正向 | 触觉设备X → 机械臂2 X |
| Y轴 | Y轴 | 正向 | 触觉设备Y → 机械臂2 Y |
| Z轴 | Z轴 | 取反 | 触觉设备Z → 机械臂2 -Z |
| RX轴 | RX轴 | 正向 | 触觉设备RX → 机械臂2 RX |
| RY轴 | RY轴 | 正向 | 触觉设备RY → 机械臂2 RY |
| RZ轴 | RZ轴 | 取反 | 触觉设备RZ → 机械臂2 -RZ |

### 键盘控制
| 按键 | 功能 |
|------|------|
| `1` / `2` | 选择调整设备1或设备2 |
| `+` / `-` | 调整位置映射系数 |
| `[` / `]` | 调整姿态映射系数 |
| `{` / `}` | 调整弹簧刚度 |
| `s` | 查询当前机械臂状态 |
| `c` | 保存配置 |
| `f` | 切换坐标系类型 |
| `q` | 退出程序 |

## ⚙️ 配置文件

新版本支持更详细的配置选项，包括独立的设备映射配置：

```ini
# === 设备1独立映射配置 ===
[device1_mapping]
# 机械臂轴符号调整 (1=正向, -1=反向)
arm_x_sign = -1   # 机械臂1 X轴符号（取反）
arm_y_sign = -1   # 机械臂1 Y轴符号（取反）
arm_z_sign = 1    # 机械臂1 Z轴符号

# 触觉设备轴到机械臂轴的映射 (0=X,1=Y,2=Z)
touch_pos_to_arm_x = 2  # 触觉设备Z轴 → 机械臂X轴
touch_pos_to_arm_y = 0  # 触觉设备X轴 → 机械臂Y轴
touch_pos_to_arm_z = 1  # 触觉设备Y轴 → 机械臂Z轴

# === 设备2独立映射配置 ===
[device2_mapping]
# 可以与设备1完全不同的映射配置
arm_x_sign = 1    # 机械臂2 X轴符号（正向）
arm_y_sign = 1    # 机械臂2 Y轴符号（正向）
arm_z_sign = -1   # 机械臂2 Z轴符号（取反）

# === 设备控制参数 ===
[device1]
position_scale = 1000.0   # 设备1位置映射系数
rotation_scale = 1.0      # 设备1姿态映射系数
spring_stiffness = 0.2    # 设备1弹簧刚度

[device2]
position_scale = 1000.0   # 设备2位置映射系数
rotation_scale = 1.0      # 设备2姿态映射系数
spring_stiffness = 0.2    # 设备2弹簧刚度

# === 机械臂连接配置 ===
[robot1]
ip = 192.168.10.18        # 机械臂1 IP
port = 8080               # 机械臂1端口

[robot2]
ip = 192.168.10.19        # 机械臂2 IP  
port = 8080               # 机械臂2端口

# === 设备名称配置 ===
[device_names]
device1_primary = PHANToM 1
device1_fallback = Device1
device2_primary = PHANToM 2
device2_fallback = Device2

# === 系统配置 ===
[system]
control_frequency = 10         # 控制命令发送频率(毫秒)
debug_frequency = 50          # 调试信息显示频率
enable_angle_transmission = true   # 启用角度透传模式
enable_arm_power = true       # 启用机械臂电源
teach_frame_type = 1          # 示教坐标系类型

# === 夹爪配置 ===
[gripper]
block_mode = true             # 夹爪控制模式
force_threshold = 200         # 夹爪力控阈值
pick_speed = 500             # 夹爪抓取速度
release_speed = 500          # 夹爪释放速度
```

## 🛠️ 编译选项

### CMake构建（推荐）
```bash
# 配置
mkdir build && cd build
cmake ..

# 编译
make -j$(nproc)

# 检查配置
make check-config

# 安装
sudo make install
```

### 传统Makefile（新版本2.0.0）
```bash
# 显示帮助
make help

# 编译
make

# 调试版本
make debug

# 发布版本
make release

# 环境检查
make check

# 运行（完整日志）
make run

# 运行（过滤hEvent调试信息）
make run-clean

# 设备配置测试
make test-devices

# 检查配置文件
make check-config

# 创建软件包
make package

# 安装到系统
sudo make install
```

### 手动编译
```bash
gcc -c conio.c -o conio.o
g++ -o Touch_Controller_Arm2 Touch_Controller_Arm2.cpp conio.o \
    -I./OpenHaptics/openhaptics_3.4-0-developer-edition-amd64/usr/include \
    -L./OpenHaptics/openhaptics_3.4-0-developer-edition-amd64/usr/lib \
    -lHD -lHDU -lrt -lpthread -lncurses -std=c++11
```

## 📚 文档

- 📖 [双设备控制说明](md/双设备控制说明.md)
- 🏗️ [构建指南](md/构建指南.md)
- 📋 [双Touch设备使用说明](md/双Touch设备使用说明.md)
- 📝 [加载说明](md/加载说明.md)
- ⚙️ [配置映射使用说明](md/config_mapping_usage.md)
- 🛡️ [容错功能测试说明](md/容错功能测试说明.md)
- 📊 [设备配置验证报告](md/设备配置验证报告.md)

## 🏗️ 项目结构

```
Touch_Controller_Arm2/
├── Touch_Controller_Arm2.cpp     # 主控制程序（v2.0.0）
├── ConfigLoader.h                # 配置文件加载器
├── conio.c / conio.h             # 控制台输入处理
├── config.ini                    # 配置文件（增强版）
├── CMakeLists.txt                # CMake配置
├── Makefile                      # 传统Makefile（v2.0.0）
├── run2.sh                       # 启动脚本（过滤调试信息）
├── OpenHaptics/                  # OpenHaptics库
├── md/                           # 文档目录
│   ├── 双设备控制说明.md
│   ├── 构建指南.md
│   ├── 双Touch设备使用说明.md
│   ├── 加载说明.md
│   ├── config_mapping_usage.md
│   ├── 容错功能测试说明.md
│   └── 设备配置验证报告.md
└── README.md                     # 本文件
```

## 🐛 故障排除

### 常见问题

1. **设备初始化失败**
   - 检查USB连接和驱动
   - 使用root权限运行
   - 运行官方Touch_Diagnostic检查

2. **机械臂连接失败**
   - ✅ **程序不会退出** - 自动切换到触觉反馈模式
   - 检查网络连接
   - 确认IP地址和端口配置
   - 测试机械臂是否响应ping
   - 重启程序以重新尝试连接

3. **编译错误**
   - 检查OpenHaptics库路径
   - 安装缺失的依赖库
   - 更新编译器版本

4. **设备映射问题**
   - 运行 `make test-devices` 测试设备配置
   - 查看配置映射使用说明
   - 检查 config.ini 中的映射配置

详细故障排除请参考 [构建指南](md/构建指南.md) 和 [容错功能测试说明](md/容错功能测试说明.md)。

## 🔧 技术架构

### 核心组件
- **ArmController**: TCP通信和机械臂控制
- **TouchArmController**: 触觉设备到机械臂的映射控制  
- **ConfigLoader**: 配置文件管理（增强版）
- **IndependentMappingSystem**: 独立设备映射系统

### 通信协议
- 触觉设备: OpenHaptics HD/HDU API
- 机械臂: TCP + JSON协议
- 配置管理: INI格式文件（增强版）

### 性能特性
- 触觉反馈频率: 1000Hz
- 控制命令频率: 可配置 (默认100Hz)
- 坐标映射: 实时转换，独立配置
- 参数调整: 实时生效

### 新增特性（v2.0.0）
- 独立设备映射配置
- 设备名称自动识别
- 增强的容错机制
- 详细的调试和日志过滤
- 设备配置验证工具

## 🤝 贡献

欢迎提交Issue和Pull Request！

1. Fork本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开Pull Request

## 📄 许可证

本项目基于MIT许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 📞 联系方式

- 项目维护者: Ruio
- 邮箱: ruio@example.com

## 🙏 致谢

- [OpenHaptics](https://www.3dsystems.com/haptics-devices/openhaptics) - 触觉设备开发框架
- [实曼机械臂](https://www.realman-robotics.com/) - 机械臂控制协议

---

⭐ 如果这个项目对您有帮助，请给个Star支持一下！ 
