#pragma once

#include <array>
#include <string>
#include <fstream>

// 默认弹簧刚度常量 - 可在这里直接修改弹簧力大小（参考SimpleHapticScene）
const double DEFAULT_SPRING_STIFFNESS = 0.2;

class DragController {
public:
    DragController();
    ~DragController();
    
    void setScale(double scale);
    double getScale() const { return m_scale; }
    
    void setSpringStiffness(double stiffness);
    double getSpringStiffness() const { return m_springStiffness; }
    
    void onButtonDown(const std::array<double,3>& pos, const std::array<double,16>& transform);
    void onButtonUp();
    
    std::string update(const std::array<double,3>& pos, const std::array<double,16>& transform);
    std::array<double,3> calculateSpringForce(const std::array<double,3>& position) const;
    
    bool isDragging() const { return m_dragging; }
    
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

private:
    bool m_dragging;
    std::array<double,3> m_anchor;
    std::array<double,16> m_anchorTransform;  // 锚点姿态变换矩阵
    double m_scale;
    double m_springStiffness;  // 弹簧刚度系数
    std::ofstream m_outputFile;
    
    std::string formatJson(const std::array<double,3>& relative, const std::array<double,3>& rotation) const;
    std::array<double,3> extractEulerAngles(const std::array<double,16>& transform) const;
}; 