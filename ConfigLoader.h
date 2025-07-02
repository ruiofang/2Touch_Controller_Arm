#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

class ConfigLoader {
private:
    std::map<std::string, std::string> m_config;
    std::string m_filename;

    // 去除字符串两端的空白字符
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

public:
    ConfigLoader(const std::string& filename) : m_filename(filename) {}

    // 加载配置文件
    bool loadConfig() {
        std::ifstream file(m_filename);
        if (!file.is_open()) {
            std::cout << "配置文件不存在，将使用默认值: " << m_filename << std::endl;
            return false;
        }

        std::string line;
        std::string currentSection = "";
        
        while (std::getline(file, line)) {
            line = trim(line);
            
            // 跳过空行和注释
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // 处理节（section）
            if (line[0] == '[' && line[line.length()-1] == ']') {
                currentSection = line.substr(1, line.length()-2);
                continue;
            }
            
            // 处理键值对
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // 如果有节名，则添加节前缀
                if (!currentSection.empty()) {
                    key = currentSection + "." + key;
                }
                
                m_config[key] = value;
            }
        }
        
        file.close();
        std::cout << "成功加载配置文件: " << m_filename << std::endl;
        return true;
    }

    // 保存配置文件
    bool saveConfig() {
        std::ofstream file(m_filename);
        if (!file.is_open()) {
            std::cerr << "无法创建配置文件: " << m_filename << std::endl;
            return false;
        }

        // 按节分组写入
        std::map<std::string, std::map<std::string, std::string>> sections;
        
        for (const auto& pair : m_config) {
            size_t pos = pair.first.find('.');
            if (pos != std::string::npos) {
                std::string section = pair.first.substr(0, pos);
                std::string key = pair.first.substr(pos + 1);
                sections[section][key] = pair.second;
            } else {
                sections[""][pair.first] = pair.second;
            }
        }

        // 写入文件
        file << "# Touch Controller Arm 配置文件" << std::endl;
        file << "# 自动生成，请谨慎修改" << std::endl;
        file << std::endl;

        for (const auto& section : sections) {
            if (!section.first.empty()) {
                file << "[" << section.first << "]" << std::endl;
            }
            
            for (const auto& pair : section.second) {
                file << pair.first << " = " << pair.second << std::endl;
            }
            file << std::endl;
        }

        file.close();
        std::cout << "配置已保存到: " << m_filename << std::endl;
        return true;
    }

    // 获取字符串值
    std::string getString(const std::string& key, const std::string& defaultValue = "") {
        auto it = m_config.find(key);
        return (it != m_config.end()) ? it->second : defaultValue;
    }

    // 获取整数值
    int getInt(const std::string& key, int defaultValue = 0) {
        auto it = m_config.find(key);
        if (it != m_config.end()) {
            try {
                return std::stoi(it->second);
            } catch (const std::exception&) {
                std::cerr << "配置项 " << key << " 不是有效的整数，使用默认值" << std::endl;
            }
        }
        return defaultValue;
    }

    // 获取双精度浮点数值
    double getDouble(const std::string& key, double defaultValue = 0.0) {
        auto it = m_config.find(key);
        if (it != m_config.end()) {
            try {
                return std::stod(it->second);
            } catch (const std::exception&) {
                std::cerr << "配置项 " << key << " 不是有效的浮点数，使用默认值" << std::endl;
            }
        }
        return defaultValue;
    }

    // 获取布尔值
    bool getBool(const std::string& key, bool defaultValue = false) {
        auto it = m_config.find(key);
        if (it != m_config.end()) {
            std::string value = it->second;
            // 转换为小写
            for (char& c : value) {
                c = std::tolower(c);
            }
            return (value == "true" || value == "1" || value == "yes" || value == "on");
        }
        return defaultValue;
    }

    // 设置值
    void setString(const std::string& key, const std::string& value) {
        m_config[key] = value;
    }

    void setInt(const std::string& key, int value) {
        m_config[key] = std::to_string(value);
    }

    void setDouble(const std::string& key, double value) {
        m_config[key] = std::to_string(value);
    }

    void setBool(const std::string& key, bool value) {
        m_config[key] = value ? "true" : "false";
    }

    // 打印所有配置
    void printConfig() {
        std::cout << "=== 当前配置 ===" << std::endl;
        for (const auto& pair : m_config) {
            std::cout << pair.first << " = " << pair.second << std::endl;
        }
        std::cout << "=================" << std::endl;
    }
};

#endif // CONFIG_LOADER_H 