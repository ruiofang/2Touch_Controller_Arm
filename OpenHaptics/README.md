# USB-HID设备读取程序

基于OpenHaptics SDK和libusb-1.0开发的USB-HID设备读取程序，参考Touch_Diagnostic实现。

## 功能特性

- **USB-HID设备枚举**: 自动发现并列出系统中的HID设备
- **3D Systems设备支持**: 特别针对3D Systems (厂商ID: 0x2988) 的触觉设备
- **OpenHaptics集成**: 同时支持OpenHaptics SDK的高级接口
- **实时数据读取**: 连续读取USB HID原始数据和触觉设备状态
- **信号处理**: 支持Ctrl+C优雅退出
- **详细日志**: 提供详细的设备信息和调试输出

## 系统要求

### 必需依赖
- **Linux系统** (Ubuntu 18.04+ 推荐)
- **OpenHaptics SDK 3.4.0+**
- **libusb-1.0-dev**
- **g++** 支持C++11

### 可选依赖
- **libPhantomIOLib42** (用于3D Systems设备的底层访问)

## 安装和编译

### 1. 安装系统依赖

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential libusb-1.0-0-dev pkg-config

# CentOS/RHEL
sudo yum install gcc-c++ libusb1-devel pkgconfig
```

### 2. 检查依赖

```bash
make check-deps
```

### 3. 编译程序

```bash
# 标准编译
make

# 调试版本
make debug

# 显示详细信息
make help
```

## 使用方法

### 基本使用

```bash
# 直接运行
./usb_hid_reader

# 或使用make运行
make run

# 如果需要权限
sudo ./usb_hid_reader
```

### 安装到系统

```bash
# 安装到 /usr/local/bin
sudo make install

# 从任何位置运行
usb_hid_reader

# 卸载
sudo make uninstall
```

## 程序输出说明

### 设备枚举阶段
程序启动后会首先枚举所有HID设备，显示：
- 厂商ID (Vendor ID)
- 产品ID (Product ID)  
- 制造商名称
- 产品名称
- 序列号

### 数据读取阶段
如果找到兼容设备，程序会显示：
- **USB HID原始数据**: 十六进制格式的原始数据包
- **触觉设备状态**: 位置、速度、按键状态等高级信息

### 示例输出

```
USB-HID设备读取程序
基于OpenHaptics SDK和libusb-1.0
==================================================

找到的HID设备列表:
================================================================================
设备 1:
  厂商ID: 0x2988
  产品ID: 0x0304
  制造商: 3D Systems
  产品名: Touch
  序列号: ABC123456
----------------------------------------

找到触觉设备: Touch
找到3D Systems设备: Touch
成功打开设备，开始读取数据...
按 Ctrl+C 退出

--- 读取 #1 ---
USB HID 数据 (64 字节):
01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10
11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20
...

触觉设备状态:
  位置: (12.34, -5.67, 8.90)
  速度: (0.12, 0.34, -0.05)
  按键: 0x1
```

## 权限配置

### USB设备权限
如果程序无法访问USB设备，需要配置udev规则：

```bash
# 复制现有的udev规则
sudo cp TouchDriver_2023_01_12/rules.d/99-3dsystems.rules /etc/udev/rules.d/

# 重新加载udev规则
sudo udevadm control --reload
sudo udevadm trigger

# 或者手动添加用户到plugdev组
sudo usermod -a -G plugdev $USER
```

### OpenHaptics权限
确保OpenHaptics驱动正确安装：

```bash
# 安装触觉驱动
cd TouchDriver_2023_01_12
sudo ./install_haptic_driver
```

## 故障排除

### 编译错误

**错误**: `HD/hd.h: No such file or directory`
**解决**: 检查OpenHaptics SDK路径，确保在正确位置

**错误**: `undefined reference to libusb_*`
**解决**: 安装libusb-1.0-dev包

### 运行时错误

**错误**: "初始化触觉设备失败"
**解决**: 
1. 检查设备连接
2. 运行Phantom Configuration工具
3. 确保驱动正确安装

**错误**: "无法打开设备，可能需要管理员权限"
**解决**:
1. 使用sudo运行
2. 配置udev规则
3. 检查设备是否被其他程序占用

### 设备检测问题

**问题**: 找不到3D Systems设备
**解决**:
1. 检查USB连接
2. 运行 `lsusb` 确认设备存在
3. 检查设备ID是否正确 (应为2988:xxxx)

## 开发和扩展

### 修改设备过滤
要支持其他厂商的HID设备，修改 `usb_hid_reader.cpp` 中的过滤条件：

```cpp
// 在 enumerateDevices() 函数中
if (isHID || desc.idVendor == 0x您的厂商ID) {
    // 添加设备
}
```

### 自定义数据解析
在 `readHIDData()` 回调中添加自定义的数据解析逻辑：

```cpp
if (usbReader.readHIDData(buffer, sizeof(buffer), 100)) {
    // 解析您的设备特定协议
    parseCustomProtocol(buffer, sizeof(buffer));
}
```

## 参考资料

- [OpenHaptics Developer Guide](openhaptics_3.4-0-developer-edition-amd64/doc/OpenHaptics_ProgGuide.pdf)
- [OpenHaptics Reference Guide](openhaptics_3.4-0-developer-edition-amd64/doc/OpenHaptics_RefGuide.pdf)
- [libusb-1.0 API Reference](https://libusb.sourceforge.io/api-1.0/)

## 许可证

本程序基于OpenHaptics SDK开发，请遵守相应的许可协议。

## 贡献

欢迎提交问题报告和改进建议！ 