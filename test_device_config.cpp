/*****************************************************************************
 触觉设备配置测试程序
 
 功能：
 - 测试不同设备名称的初始化
 - 验证PHANToM 1和PHANToM 2设备配置
 - 显示设备信息和连接状态
 - 不启动触觉调度器，仅用于设备检测
*****************************************************************************/

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#if defined(WIN32)
# include <windows.h>
#else
# include <unistd.h>
#endif

#define linux 1
#include <HD/hd.h>
#include <HDU/hduError.h>
#include "ConfigLoader.h"

// 测试设备初始化函数
bool testDeviceInit(const std::string& deviceName, const std::string& description) {
    std::cout << "\n=== 测试设备: " << description << " (" << deviceName << ") ===" << std::endl;
    
    HDErrorInfo error;
    HHD hDevice = HD_INVALID_HANDLE;
    
    if (deviceName == "Default Device") {
        hDevice = hdInitDevice(HD_DEFAULT_DEVICE);
    } else {
        hDevice = hdInitDevice(deviceName.c_str());
    }
    
    if (HD_DEVICE_ERROR(error = hdGetError())) {
        std::cout << "❌ 设备初始化失败: " << deviceName << std::endl;
        std::cout << "   错误信息: ";
        hduPrintError(stdout, &error, "");
        std::cout << std::endl;
        return false;
    } else {
        hdMakeCurrentDevice(hDevice);
        std::cout << "✅ 设备初始化成功!" << std::endl;
        std::cout << "   设备型号: " << hdGetString(HD_DEVICE_MODEL_TYPE) << std::endl;
        std::cout << "   序列号: " << hdGetString(HD_DEVICE_SERIAL_NUMBER) << std::endl;
        std::cout << "   驱动版本: " << hdGetString(HD_DEVICE_DRIVER_VERSION) << std::endl;
        std::cout << "   厂商: " << hdGetString(HD_DEVICE_VENDOR) << std::endl;
        
        // 获取设备工作空间信息
        HDdouble maxWorkspace[6];
        HDdouble usableWorkspace[6];
        hdGetDoublev(HD_MAX_WORKSPACE_DIMENSIONS, maxWorkspace);
        hdGetDoublev(HD_USABLE_WORKSPACE_DIMENSIONS, usableWorkspace);
        
        std::cout << "   最大工作空间: [" << maxWorkspace[0] << ", " << maxWorkspace[1] << ", " << maxWorkspace[2] << "]" << std::endl;
        std::cout << "   可用工作空间: [" << usableWorkspace[0] << ", " << usableWorkspace[1] << ", " << usableWorkspace[2] << "]" << std::endl;
        
        // 获取设备能力
        HDint inputDOF, outputDOF;
        hdGetIntegerv(HD_INPUT_DOF, &inputDOF);
        hdGetIntegerv(HD_OUTPUT_DOF, &outputDOF);
        std::cout << "   输入自由度: " << inputDOF << ", 输出自由度: " << outputDOF << std::endl;
        
        // 清理设备
        hdDisableDevice(hDevice);
        return true;
    }
}

int main() {
    std::cout << "=== 触觉设备配置测试程序 ===" << std::endl;
    std::cout << "基于OpenHaptics官方HelloSphereDual示例的设备配置测试" << std::endl;
    
    // 加载配置文件
    ConfigLoader config("config.ini");
    config.loadConfig();
    
    std::cout << "\n=== 配置文件设备名称 ===" << std::endl;
    std::string device1Primary = config.getString("device_names.device1_primary", "PHANToM 1");
    std::string device1Fallback = config.getString("device_names.device1_fallback", "Default Device");
    std::string device2Primary = config.getString("device_names.device2_primary", "PHANToM 2");
    std::string device2Fallback = config.getString("device_names.device2_fallback", "Device1,PHANTOM 2,PHANToM Device 2");
    
    std::cout << "设备1首选: " << device1Primary << std::endl;
    std::cout << "设备1备用: " << device1Fallback << std::endl;
    std::cout << "设备2首选: " << device2Primary << std::endl;
    std::cout << "设备2备用: " << device2Fallback << std::endl;
    
    // 测试标准OpenHaptics设备名称
    std::cout << "\n=== OpenHaptics标准设备名称测试 ===" << std::endl;
    
    bool device1Found = testDeviceInit(device1Primary, "设备1首选");
    if (!device1Found) {
        std::cout << "\n尝试设备1备用名称..." << std::endl;
        device1Found = testDeviceInit(device1Fallback, "设备1备用");
    }
    
    bool device2Found = testDeviceInit(device2Primary, "设备2首选");
    if (!device2Found) {
        std::cout << "\n尝试设备2备用名称..." << std::endl;
        
        // 解析逗号分隔的备用设备名称
        std::istringstream fallbackStream(device2Fallback);
        std::string deviceName;
        
        while (std::getline(fallbackStream, deviceName, ',') && !device2Found) {
            // 移除前后空格
            deviceName.erase(0, deviceName.find_first_not_of(" \t"));
            deviceName.erase(deviceName.find_last_not_of(" \t") + 1);
            
            if (!deviceName.empty()) {
                device2Found = testDeviceInit(deviceName, "设备2备用 - " + deviceName);
            }
        }
    }
    
    // 额外测试其他常见设备名称
    std::cout << "\n=== 其他常见设备名称测试 ===" << std::endl;
    
    std::vector<std::pair<std::string, std::string>> additionalDevices = {
        {"Default Device", "系统默认设备"},
        {"PHANToM Desktop", "PHANToM Desktop设备"},
        {"PHANToM Omni", "PHANToM Omni设备"},
        {"PHANTOM Premium", "PHANTOM Premium设备"},
        {"Touch", "Touch设备"},
        {"Touch Device", "Touch Device"},
        {"Geomagic Touch", "Geomagic Touch设备"}
    };
    
    for (const auto& device : additionalDevices) {
        // 跳过已经测试过的设备
        if (device.first == device1Primary || device.first == device2Primary || 
            device.first == device1Fallback || device2Fallback.find(device.first) != std::string::npos) {
            continue;
        }
        testDeviceInit(device.first, device.second);
    }
    
    // 显示总结
    std::cout << "\n=== 测试总结 ===" << std::endl;
    std::cout << "设备1 (" << device1Primary << "): " << (device1Found ? "✅ 可用" : "❌ 不可用") << std::endl;
    std::cout << "设备2 (" << device2Primary << "): " << (device2Found ? "✅ 可用" : "❌ 不可用") << std::endl;
    
    if (device1Found && device2Found) {
        std::cout << "\n🎉 双设备配置验证成功！" << std::endl;
        std::cout << "可以运行Touch_Controller_Arm程序进行双设备控制" << std::endl;
    } else if (device1Found || device2Found) {
        std::cout << "\n⚠️  单设备配置可用" << std::endl;
        std::cout << "Touch_Controller_Arm程序将以单设备模式运行" << std::endl;
    } else {
        std::cout << "\n❌ 未找到任何可用的触觉设备" << std::endl;
        std::cout << "请检查设备连接和OpenHaptics驱动安装" << std::endl;
        std::cout << "建议运行Touch_Setup和Touch_Diagnostic检查设备状态" << std::endl;
    }
    
    std::cout << "\n=== 设备配置建议 ===" << std::endl;
    std::cout << "1. 确保两个设备都已正确连接到USB端口" << std::endl;
    std::cout << "2. 运行Touch_Setup配置每个设备的名称" << std::endl;
    std::cout << "3. 在config.ini中设置正确的device1_primary和device2_primary" << std::endl;
    std::cout << "4. 如果使用非标准设备名称，请更新device1_fallback和device2_fallback" << std::endl;
    
    return 0;
} 