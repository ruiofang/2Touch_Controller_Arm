#include "DragController.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

DragController::DragController() 
    : m_dragging(false), m_scale(1.0), m_springStiffness(DEFAULT_SPRING_STIFFNESS), 
      m_anchor({0.0, 0.0, 0.0}), m_anchorTransform({1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0})
{
    // 打开输出文件
    m_outputFile.open("drag_output.json", std::ios::app);
    if (!m_outputFile.is_open()) {
        std::cerr << "警告: 无法打开输出文件 drag_output.json" << std::endl;
    }
}

DragController::~DragController()
{
    if (m_outputFile.is_open()) {
        m_outputFile.close();
    }
}

void DragController::setScale(double scale)
{
    m_scale = scale;
    std::cout << "映射系数设置为: " << scale << std::endl;
}

void DragController::setSpringStiffness(double stiffness)
{
    m_springStiffness = stiffness;
    std::cout << "弹簧刚度设置为: " << stiffness << std::endl;
}

void DragController::onButtonDown(const std::array<double,3>& pos, const std::array<double,16>& transform)
{
    if (!m_dragging) {
        m_anchor = pos;
        m_anchorTransform = transform;
        m_dragging = true;
        
        std::array<double,3> eulerAngles = extractEulerAngles(transform);
        std::cout << "开始拖动 - 锚点设置为: [" 
                  << std::fixed << std::setprecision(4)
                  << pos[0] << ", " << pos[1] << ", " << pos[2] << "]" << std::endl;
        std::cout << "锚点姿态 (度): [" 
                  << std::fixed << std::setprecision(2)
                  << eulerAngles[0] << ", " << eulerAngles[1] << ", " << eulerAngles[2] << "]" << std::endl;
        
        // 记录开始时间到文件
        if (m_outputFile.is_open()) {
            std::time_t now = std::time(0);
            m_outputFile << "# 拖动开始: " << std::ctime(&now);
        }
    }
}

void DragController::onButtonUp()
{
    if (m_dragging) {
        m_dragging = false;
        std::cout << "结束拖动" << std::endl;
        
        // 记录结束时间到文件
        if (m_outputFile.is_open()) {
            std::time_t now = std::time(0);
            m_outputFile << "# 拖动结束: " << std::ctime(&now) << std::endl;
            m_outputFile.flush();
        }
    }
}

std::string DragController::update(const std::array<double,3>& pos, const std::array<double,16>& transform)
{
    if (!m_dragging) {
        return "";
    }
    
    // 计算相对坐标
    std::array<double,3> relative = {
        (pos[0] - m_anchor[0]) * m_scale,
        (pos[1] - m_anchor[1]) * m_scale,
        (pos[2] - m_anchor[2]) * m_scale
    };
    
    // 计算相对姿态（欧拉角）
    std::array<double,3> currentEuler = extractEulerAngles(transform);
    std::array<double,3> anchorEuler = extractEulerAngles(m_anchorTransform);
    std::array<double,3> relativeRotation = {
        (currentEuler[0] - anchorEuler[0]) * m_scale,
        (currentEuler[1] - anchorEuler[1]) * m_scale,
        (currentEuler[2] - anchorEuler[2]) * m_scale
    };
    
    std::string json = formatJson(relative, relativeRotation);
    
    // 计算位置距离（用于判断是否需要输出）
    double distance = std::sqrt(relative[0]*relative[0] + relative[1]*relative[1] + relative[2]*relative[2]);
    
    // 计算姿态变化量
    double rotationChange = std::sqrt(relativeRotation[0]*relativeRotation[0] + 
                                     relativeRotation[1]*relativeRotation[1] + 
                                     relativeRotation[2]*relativeRotation[2]);
    
    // 只有当移动距离或姿态变化超过阈值时才输出（避免噪声）
    static int outputCounter = 0;
    outputCounter++;
    
    // 每10帧输出一次，或者移动/旋转距离较大时立即输出
    if (outputCounter >= 10 || distance > 0.5 || rotationChange > 1.0) {
        // 输出到控制台（换行显示，更清晰）
        printf("相对位置: [%.3f, %.3f, %.3f] 距离: %.3f mm\n", 
               relative[0], relative[1], relative[2], distance);
        printf("相对姿态: [%.2f°, %.2f°, %.2f°] 变化: %.2f°\n", 
               relativeRotation[0], relativeRotation[1], relativeRotation[2], rotationChange);
        
        // 写入文件
        if (m_outputFile.is_open()) {
            m_outputFile << json << std::endl;
            m_outputFile.flush(); // 立即刷新到磁盘
        }
        
        outputCounter = 0; // 重置计数器
    }
    
    return json;
}

std::array<double,3> DragController::calculateSpringForce(const std::array<double,3>& position) const
{
    // 使用与SimpleHapticScene完全相同的弹簧力公式
    // F = k * (anchor - position)  力指向锚点
    std::array<double,3> springForce = {
        m_springStiffness * (m_anchor[0] - position[0]),
        m_springStiffness * (m_anchor[1] - position[1]),
        m_springStiffness * (m_anchor[2] - position[2])
    };
    
    return springForce;
}

std::string DragController::formatJson(const std::array<double,3>& relative, const std::array<double,3>& rotation) const
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    oss << "{ \"relative_position\": [";
    for (int i = 0; i < 3; ++i) {
        if (i > 0) oss << ", ";
        oss << relative[i];
    }
    oss << "], \"relative_rotation\": [";
    for (int i = 0; i < 3; ++i) {
        if (i > 0) oss << ", ";
        oss << rotation[i];
    }
    oss << "], \"scale\": " << m_scale;
    oss << ", \"timestamp\": " << std::time(nullptr) << " }";
    return oss.str();
}

std::array<double,3> DragController::extractEulerAngles(const std::array<double,16>& transform) const
{
    // 从4x4变换矩阵提取欧拉角（XYZ顺序）
    // 变换矩阵是列主序排列
    double r11 = transform[0];  // m[0][0]
    double r12 = transform[4];  // m[0][1]
    double r13 = transform[8];  // m[0][2]
    double r21 = transform[1];  // m[1][0]
    double r22 = transform[5];  // m[1][1]
    double r23 = transform[9];  // m[1][2]
    double r31 = transform[2];  // m[2][0]
    double r32 = transform[6];  // m[2][1]
    double r33 = transform[10]; // m[2][2]
    
    double roll, pitch, yaw;
    
    // 提取欧拉角（以度为单位）
    pitch = std::asin(-r31);
    
    if (std::cos(pitch) > 0.0001) {
        roll = std::atan2(r32, r33);
        yaw = std::atan2(r21, r11);
    } else {
        roll = std::atan2(-r23, r22);
        yaw = 0.0;
    }
    
    // 转换为度
    roll = roll * 180.0 / M_PI;
    pitch = pitch * 180.0 / M_PI;
    yaw = yaw * 180.0 / M_PI;
    
    return {roll, pitch, yaw};
}

void DragController::saveToFile(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "{ \"scale\": " << m_scale << " }" << std::endl;
        file.close();
        std::cout << "配置已保存到: " << filename << std::endl;
    } else {
        std::cerr << "无法保存配置到文件: " << filename << std::endl;
    }
}

void DragController::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (file.is_open()) {
        // 简单的配置读取 (实际应用中可使用JSON库)
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("\"scale\":") != std::string::npos) {
                size_t pos = line.find(":");
                if (pos != std::string::npos) {
                    std::string value = line.substr(pos + 1);
                    // 移除空格和非数字字符
                    value.erase(0, value.find_first_of("0123456789.-"));
                    value.erase(value.find_last_of("0123456789.") + 1);
                    m_scale = std::stod(value);
                    std::cout << "从文件加载映射系数: " << m_scale << std::endl;
                    break;
                }
            }
        }
        file.close();
    }
} 