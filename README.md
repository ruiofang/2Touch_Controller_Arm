# 双Touch设备双机械臂控制系统

[![构建状态](https://img.shields.io/badge/构建-成功-brightgreen)]()
[![版本](https://img.shields.io/badge/版本-1.0.0-blue)]()
[![许可证](https://img.shields.io/badge/许可证-MIT-green)]()

## 🎯 项目概述

这是一个基于OpenHaptics的**双Touch设备双机械臂控制系统**，支持同时使用两个触觉设备分别控制两个机械臂，实现真正的双臂协同操作。

### ✨ 主要特性

- 🤖 **双机械臂控制**: 同时控制两个机械臂
- 👥 **双设备支持**: 支持两个Touch触觉设备独立操作
- 🛡️ **高度容错**: 机械臂连接失败时Touch设备仍可正常工作
- 🎮 **直观控制**: 按钮控制位置/姿态和夹抓操作
- 🔄 **实时反馈**: 高频率触觉力反馈 (1000Hz)
- 🌐 **TCP通信**: 基于JSON协议的机械臂通信
- ⚙️ **参数可调**: 实时调整映射系数和反馈强度
- 📝 **配置保存**: 自动保存用户偏好设置

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
cd 2Touch_Controller_Arm
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

# 方式3: 使用启动脚本
./启动双机械臂控制.sh
```

## 📋 控制映射

### 设备控制
| 设备 | 按钮1 | 按钮2 |
|------|-------|-------|
| 设备1 | 机械臂1位置姿态 | 机械臂1夹抓 |
| 设备2 | 机械臂2位置姿态 | 机械臂2夹抓 |

### 坐标映射
| 触觉设备 | 机械臂 | 说明 |
|----------|--------|------|
| X轴 | X轴 | 位置直接对应 |
| Y轴 | Y轴 | 位置直接对应 |
| Z轴 | Z轴 | 位置直接对应 |
| RX轴 | -RX轴 | 姿态取反 |
| RY轴 | -RY轴 | 姿态取反 |
| RZ轴 | RZ轴 | 姿态直接对应 |

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

```ini
[device1]
position_scale = 1240.578978  # 设备1位置映射系数
rotation_scale = 0.774841     # 设备1姿态映射系数
spring_stiffness = 0.2        # 设备1弹簧刚度

[device2]
position_scale = 1000.0       # 设备2位置映射系数
rotation_scale = 2.0          # 设备2姿态映射系数
spring_stiffness = 0.2        # 设备2弹簧刚度

[robot1]
ip = 192.168.10.19           # 机械臂1 IP
port = 8080                  # 机械臂1端口

[robot2]
ip = 192.168.10.18           # 机械臂2 IP  
port = 8080                  # 机械臂2端口
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

### 传统Makefile
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
```

### 手动编译
```bash
gcc -c conio.c -o conio.o
g++ -o Touch_Controller_Arm Touch_Controller_Arm.cpp conio.o \
    -I./OpenHaptics/openhaptics_3.4-0-developer-edition-amd64/usr/include \
    -L./OpenHaptics/openhaptics_3.4-0-developer-edition-amd64/usr/lib \
    -lHD -lHDU -lrt -lpthread -lncurses -std=c++11
```

## 📚 文档

- 📖 [详细控制说明](双设备控制说明.md)
- 🏗️ [构建指南](构建指南.md)
- 📋 [原有功能说明](双Touch设备使用说明.md)
- 📝 [安装说明](加载说明.md)

## 🏗️ 项目结构

```
2Touch_Controller_Arm/
├── Touch_Controller_Arm.cpp      # 主控制程序
├── ConfigLoader.h                # 配置文件加载器
├── conio.c / conio.h             # 控制台输入处理
├── config.ini                    # 配置文件
├── CMakeLists.txt                # CMake配置
├── Makefile                      # 传统Makefile
├── 启动双机械臂控制.sh            # 启动脚本
├── OpenHaptics/                  # OpenHaptics库
├── docs/                         # 文档目录
│   ├── 双设备控制说明.md
│   ├── 构建指南.md
│   └── 双Touch设备使用说明.md
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

详细故障排除请参考 [构建指南](构建指南.md)。

## 🔧 技术架构

### 核心组件
- **ArmController**: TCP通信和机械臂控制
- **TouchArmController**: 触觉设备到机械臂的映射控制  
- **ConfigLoader**: 配置文件管理

### 通信协议
- 触觉设备: OpenHaptics HD/HDU API
- 机械臂: TCP + JSON协议
- 配置管理: INI格式文件

### 性能特性
- 触觉反馈频率: 1000Hz
- 控制命令频率: 100Hz  
- 坐标映射: 实时转换
- 参数调整: 实时生效


sudo mkdir -p /usr/share/3DSystems/config
sudo chmod 777 /usr/share/3DSystems/config



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
