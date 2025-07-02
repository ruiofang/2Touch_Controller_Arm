/*****************************************************************************
 USB-HID TOUCH设备ID读取程序
 
 功能：
 - 扫描并列出所有USB-HID设备
 - 识别TOUCH相关的设备
 - 显示设备的详细信息，包括VID、PID、制造商、产品名称等
 
 编译命令：
 g++ -o usb_touch_device_reader usb_touch_device_reader.cpp -lusb-1.0
 
 运行命令：
 sudo ./usb_touch_device_reader
*****************************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <signal.h>

// Linux USB库
#include <libusb-1.0/libusb.h>

class USBTouchDeviceReader {
private:
    libusb_context* m_ctx;
    bool m_initialized;
    
public:
    USBTouchDeviceReader() : m_ctx(nullptr), m_initialized(false) {
        // 初始化libusb
        int result = libusb_init(&m_ctx);
        if (result != LIBUSB_SUCCESS) {
            std::cerr << "❌ 初始化libusb失败: " << libusb_error_name(result) << std::endl;
            return;
        }
        m_initialized = true;
        std::cout << "✅ libusb初始化成功" << std::endl;
    }
    
    ~USBTouchDeviceReader() {
        if (m_initialized && m_ctx) {
            libusb_exit(m_ctx);
            std::cout << "🔌 libusb已清理" << std::endl;
        }
    }
    
    bool isInitialized() const {
        return m_initialized;
    }
    
    // 检查设备是否为TOUCH相关设备
    bool isTouchDevice(libusb_device* device) {
        libusb_device_descriptor desc;
        int result = libusb_get_device_descriptor(device, &desc);
        if (result != LIBUSB_SUCCESS) {
            return false;
        }
        
        // 获取制造商和产品字符串
        libusb_device_handle* handle;
        result = libusb_open(device, &handle);
        if (result == LIBUSB_SUCCESS) {
            char manufacturer[256] = {0};
            char product[256] = {0};
            
            // 获取制造商字符串
            if (desc.iManufacturer > 0) {
                libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, 
                                                 (unsigned char*)manufacturer, sizeof(manufacturer));
            }
            
            // 获取产品字符串
            if (desc.iProduct > 0) {
                libusb_get_string_descriptor_ascii(handle, desc.iProduct, 
                                                 (unsigned char*)product, sizeof(product));
            }
            
            libusb_close(handle);
            
            // 转换为小写进行比较
            std::string mfg_lower = toLowerString(manufacturer);
            std::string prod_lower = toLowerString(product);
            
            // 检查是否包含TOUCH相关关键词
            std::vector<std::string> touch_keywords = {
                "touch", "haptic", "phantom", "omni", "force", "feedback",
                "3d", "3dsystems", "sensable", "geomagic", "novint",
                "falcon", "delta", "omega", "sigma", "tau"
            };
            
            for (const auto& keyword : touch_keywords) {
                if (mfg_lower.find(keyword) != std::string::npos || 
                    prod_lower.find(keyword) != std::string::npos) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    // 获取设备详细信息
    void printDeviceInfo(libusb_device* device, int index) {
        libusb_device_descriptor desc;
        int result = libusb_get_device_descriptor(device, &desc);
        if (result != LIBUSB_SUCCESS) {
            std::cerr << "❌ 获取设备描述符失败: " << libusb_error_name(result) << std::endl;
            return;
        }
        
        // 获取设备地址和总线号
        uint8_t bus = libusb_get_bus_number(device);
        uint8_t address = libusb_get_device_address(device);
        
        std::cout << "\n🔍 设备 #" << index << " 详细信息:" << std::endl;
        std::cout << "   总线: " << (int)bus << ", 地址: " << (int)address << std::endl;
        std::cout << "   VID: 0x" << std::hex << std::setw(4) << std::setfill('0') << desc.idVendor << std::dec << std::endl;
        std::cout << "   PID: 0x" << std::hex << std::setw(4) << std::setfill('0') << desc.idProduct << std::dec << std::endl;
        std::cout << "   设备版本: " << ((desc.bcdDevice >> 8) & 0xFF) << "." 
                  << (desc.bcdDevice & 0xFF) << std::endl;
        
        // 尝试获取制造商和产品字符串
        libusb_device_handle* handle;
        result = libusb_open(device, &handle);
        if (result == LIBUSB_SUCCESS) {
            char manufacturer[256] = {0};
            char product[256] = {0};
            char serial[256] = {0};
            
            // 获取制造商字符串
            if (desc.iManufacturer > 0) {
                result = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, 
                                                          (unsigned char*)manufacturer, sizeof(manufacturer));
                if (result > 0) {
                    std::cout << "   制造商: " << manufacturer << std::endl;
                }
            }
            
            // 获取产品字符串
            if (desc.iProduct > 0) {
                result = libusb_get_string_descriptor_ascii(handle, desc.iProduct, 
                                                          (unsigned char*)product, sizeof(product));
                if (result > 0) {
                    std::cout << "   产品名称: " << product << std::endl;
                }
            }
            
            // 获取序列号
            if (desc.iSerialNumber > 0) {
                result = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, 
                                                          (unsigned char*)serial, sizeof(serial));
                if (result > 0) {
                    std::cout << "   序列号: " << serial << std::endl;
                }
            }
            
            libusb_close(handle);
        }
        
        std::cout << "   " << std::string(50, '-') << std::endl;
    }
    
    // 扫描所有USB设备
    void scanAllDevices() {
        if (!m_initialized) {
            std::cerr << "❌ libusb未初始化" << std::endl;
            return;
        }
        
        libusb_device** list;
        ssize_t count = libusb_get_device_list(m_ctx, &list);
        if (count < 0) {
            std::cerr << "❌ 获取设备列表失败: " << libusb_error_name(count) << std::endl;
            return;
        }
        
        std::cout << "\n=== USB设备扫描结果 ===" << std::endl;
        std::cout << "发现 " << count << " 个USB设备" << std::endl;
        
        int touch_device_count = 0;
        int total_device_count = 0;
        
        for (ssize_t i = 0; i < count; i++) {
            libusb_device* device = list[i];
            total_device_count++;
            
            // 检查是否为TOUCH设备
            if (isTouchDevice(device)) {
                touch_device_count++;
                std::cout << "\n🎯 发现TOUCH设备 #" << touch_device_count << ":" << std::endl;
                printDeviceInfo(device, touch_device_count);
            }
        }
        
        std::cout << "\n=== 扫描总结 ===" << std::endl;
        std::cout << "总设备数: " << total_device_count << std::endl;
        std::cout << "TOUCH设备数: " << touch_device_count << std::endl;
        
        if (touch_device_count == 0) {
            std::cout << "⚠️  未发现TOUCH相关设备" << std::endl;
            std::cout << "提示: 请检查设备是否正确连接，或尝试以root权限运行" << std::endl;
        }
        
        libusb_free_device_list(list, 1);
    }
    
    // 扫描所有HID设备
    void scanHIDDevices() {
        if (!m_initialized) {
            std::cerr << "❌ libusb未初始化" << std::endl;
            return;
        }
        
        libusb_device** list;
        ssize_t count = libusb_get_device_list(m_ctx, &list);
        if (count < 0) {
            std::cerr << "❌ 获取设备列表失败: " << libusb_error_name(count) << std::endl;
            return;
        }
        
        std::cout << "\n=== HID设备扫描结果 ===" << std::endl;
        
        int hid_device_count = 0;
        
        for (ssize_t i = 0; i < count; i++) {
            libusb_device* device = list[i];
            
            // 检查是否为HID设备
            libusb_device_descriptor desc;
            int result = libusb_get_device_descriptor(device, &desc);
            if (result == LIBUSB_SUCCESS && desc.bDeviceClass == LIBUSB_CLASS_HID) {
                hid_device_count++;
                
                std::cout << "\n🖱️  HID设备 #" << hid_device_count << ":" << std::endl;
                
                // 获取基本信息
                uint8_t bus = libusb_get_bus_number(device);
                uint8_t address = libusb_get_device_address(device);
                
                std::cout << "   总线: " << (int)bus << ", 地址: " << (int)address << std::endl;
                std::cout << "   VID: 0x" << std::hex << std::setw(4) << std::setfill('0') << desc.idVendor << std::dec << std::endl;
                std::cout << "   PID: 0x" << std::hex << std::setw(4) << std::setfill('0') << desc.idProduct << std::dec << std::endl;
                
                // 获取产品信息
                libusb_device_handle* handle;
                result = libusb_open(device, &handle);
                if (result == LIBUSB_SUCCESS) {
                    char manufacturer[256] = {0};
                    char product[256] = {0};
                    
                    if (desc.iManufacturer > 0) {
                        libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, 
                                                         (unsigned char*)manufacturer, sizeof(manufacturer));
                        std::cout << "   制造商: " << manufacturer << std::endl;
                    }
                    
                    if (desc.iProduct > 0) {
                        libusb_get_string_descriptor_ascii(handle, desc.iProduct, 
                                                         (unsigned char*)product, sizeof(product));
                        std::cout << "   产品名称: " << product << std::endl;
                    }
                    
                    libusb_close(handle);
                }
                
                // 检查是否为TOUCH设备
                if (isTouchDevice(device)) {
                    std::cout << "   🎯 识别为TOUCH设备" << std::endl;
                }
                
                std::cout << "   " << std::string(30, '-') << std::endl;
            }
        }
        
        std::cout << "\n=== HID设备总结 ===" << std::endl;
        std::cout << "HID设备总数: " << hid_device_count << std::endl;
        
        libusb_free_device_list(list, 1);
    }
    
private:
    // 字符串转小写
    std::string toLowerString(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
};

// 信号处理函数
void signalHandler(int signum) {
    std::cout << "\n\n🛑 收到中断信号，程序退出..." << std::endl;
    exit(signum);
}

int main(int argc, char* argv[]) {
    std::cout << "=== USB-HID TOUCH设备ID读取程序 ===" << std::endl;
    std::cout << "版本: 1.0" << std::endl;
    std::cout << "功能: 扫描并识别USB-HID TOUCH设备" << std::endl;
    std::cout << "================================\n" << std::endl;
    
    // 注册信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // 创建USB设备读取器
    USBTouchDeviceReader reader;
    
    if (!reader.isInitialized()) {
        std::cerr << "❌ 程序初始化失败" << std::endl;
        return -1;
    }
    
    // 解析命令行参数
    bool scan_all = false;
    bool scan_hid = false;
    
    if (argc == 1) {
        // 默认行为：扫描TOUCH设备
        scan_all = true;
    } else {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--all" || arg == "-a") {
                scan_all = true;
            } else if (arg == "--hid" || arg == "-h") {
                scan_hid = true;
            } else if (arg == "--help" || arg == "-?") {
                std::cout << "用法: " << argv[0] << " [选项]" << std::endl;
                std::cout << "选项:" << std::endl;
                std::cout << "  --all, -a     扫描所有USB设备" << std::endl;
                std::cout << "  --hid, -h     扫描所有HID设备" << std::endl;
                std::cout << "  --help, -?    显示此帮助信息" << std::endl;
                std::cout << std::endl;
                std::cout << "示例:" << std::endl;
                std::cout << "  " << argv[0] << "              # 扫描TOUCH设备" << std::endl;
                std::cout << "  " << argv[0] << " --all         # 扫描所有USB设备" << std::endl;
                std::cout << "  " << argv[0] << " --hid         # 扫描所有HID设备" << std::endl;
                return 0;
            }
        }
    }
    
    // 执行相应的操作
    if (scan_all) {
        reader.scanAllDevices();
    }
    
    if (scan_hid) {
        reader.scanHIDDevices();
    }
    
    if (!scan_all && !scan_hid) {
        // 默认扫描TOUCH设备
        reader.scanAllDevices();
    }
    
    std::cout << "\n✅ 程序执行完成" << std::endl;
    return 0;
} 