# 坐标映射配置使用说明

## 📋 **问题解决**

原配置文件中存在冗余的通用 `[mapping]` 节，已被移除以避免混淆。

### ❌ **之前的问题**
- 通用 `[mapping]` 节永远不会被使用
- 造成配置文件重复和混乱
- 用户不清楚哪个配置会生效

### ✅ **优化后的配置**
- 只保留 `[device1_mapping]` 和 `[device2_mapping]` 节
- 配置文件更清晰、无冗余
- 每个设备的映射配置明确独立

## 🎯 **当前配置结构**

### **设备1配置** (`[device1_mapping]`)
```ini
[device1_mapping]
# 符号调整
arm_x_sign = -1   # X轴取反
arm_y_sign = -1   # Y轴取反  
arm_z_sign = 1    # Z轴正向
arm_rx_sign = -1  # RX轴取反
arm_ry_sign = -1  # RY轴取反
arm_rz_sign = 1   # RZ轴正向

# 位置映射: 触觉设备轴索引 → 机械臂轴
touch_pos_to_arm_x = 2  # 触觉Z轴(2) → 机械臂X轴
touch_pos_to_arm_y = 0  # 触觉X轴(0) → 机械臂Y轴
touch_pos_to_arm_z = 1  # 触觉Y轴(1) → 机械臂Z轴

# 姿态映射: 触觉设备旋转轴索引 → 机械臂旋转轴
touch_rot_to_arm_rx = 2  # 触觉RZ轴(2) → 机械臂RX轴
touch_rot_to_arm_ry = 0  # 触觉RX轴(0) → 机械臂RY轴
touch_rot_to_arm_rz = 1  # 触觉RY轴(1) → 机械臂RZ轴
```

### **设备2配置** (`[device2_mapping]`)
```ini
[device2_mapping]
# 符号调整（与设备1不同）
arm_x_sign = 1    # X轴正向
arm_y_sign = 1    # Y轴正向
arm_z_sign = -1   # Z轴取反
arm_rx_sign = 1   # RX轴正向
arm_ry_sign = 1   # RY轴正向
arm_rz_sign = -1  # RZ轴取反

# 位置映射（与设备1不同）
touch_pos_to_arm_x = 0  # 触觉X轴(0) → 机械臂X轴
touch_pos_to_arm_y = 1  # 触觉Y轴(1) → 机械臂Y轴
touch_pos_to_arm_z = 2  # 触觉Z轴(2) → 机械臂Z轴

# 姿态映射（与设备1不同）
touch_rot_to_arm_rx = 0  # 触觉RX轴(0) → 机械臂RX轴
touch_rot_to_arm_ry = 1  # 触觉RY轴(1) → 机械臂RY轴
touch_rot_to_arm_rz = 2  # 触觉RZ轴(2) → 机械臂RZ轴
```

## 🔧 **配置参数说明**

### **轴索引定义**
- **位置轴**: `0=X`, `1=Y`, `2=Z`
- **旋转轴**: `0=RX`, `1=RY`, `2=RZ`

### **符号调整**
- `1`: 正向映射
- `-1`: 反向映射（取反）

### **映射示例**
```ini
touch_pos_to_arm_x = 2  # 表示触觉设备的Z轴映射到机械臂的X轴
arm_x_sign = -1         # 表示机械臂X轴方向取反
```

## 🎮 **运行时操作**

### **查看当前配置**
运行程序后按 `m` 键可查看当前的坐标映射配置：

```bash
./Touch_Controller_Arm2
# 按 m 键显示配置
# 按 q 键退出
```

### **实时参数调整**
- `1`/`2`: 选择设备1或设备2
- `+`/`-`: 调整位置映射系数
- `[`/`]`: 调整姿态映射系数
- `{`/`}`: 调整弹簧刚度
- `s`: 查询机械臂状态
- `c`: 保存配置
- `f`: 切换坐标系类型

## ✅ **优势总结**

1. **配置清晰**: 每个设备有独立的映射配置节
2. **避免混淆**: 移除了不会被使用的通用配置
3. **灵活独立**: 两个设备可以有完全不同的映射关系
4. **易于维护**: 配置文件结构简洁明了
5. **实时调试**: 支持运行时查看和调整配置 