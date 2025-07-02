/*****************************************************************************
 触觉设备拖动坐标控制程序
 
 功能：
 - 通过触觉设备按钮控制拖动状态
 - 按下按钮时记录锚点，开始输出相对坐标
 - 松开按钮时停止输出
 - 相对坐标可通过映射系数调整
 - 实时输出JSON格式数据到控制台和文件
 
 操作说明：
 - 按住触觉设备按钮开始拖动
 - 移动设备查看相对坐标变化
 - 松开按钮停止拖动
 - 键盘 's' 保存配置
 - 键盘 'l' 加载配置
 - 键盘 '+'/'-' 调整映射系数
 - 键盘 'q' 退出程序
*****************************************************************************/

#include <stdio.h>
#include <iostream>
#include <array>
#include <cmath>

#if defined(WIN32)
# include <windows.h>
# include <conio.h>
#else
# include <unistd.h>
extern "C" {
# include "conio.h"
}
# include <string.h>
#endif

#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

#include "DragController.h"

// 全局变量
DragController g_dragController;
bool g_applicationRunning = true;
int g_selectedButton = HD_DEVICE_BUTTON_1; // 默认使用第一个按钮

// 夹抓控制相关变量
bool g_isGripping = false;        // 夹抓状态
bool g_lastButtonState = false;   // 上一次按钮状态
int g_buttonPressCounter = 0;     // 按钮按下计数器
const int SHORT_PRESS_MIN = 1;    // 短按最小阈值（最低设置，几乎立即触发）
const int LONG_PRESS_THRESHOLD = 50; // 长按阈值（约0.05秒，更容易区分短长按）

HDCallbackCode HDCALLBACK deviceCallback(void *data);
void handleKeyboard();
void printInstructions();

/*******************************************************************************
 主函数
*******************************************************************************/
int main(int argc, char* argv[])
{    
    HDErrorInfo error;
    HDSchedulerHandle hCallback;

    // 初始化设备
    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError())) 
    {
        hduPrintError(stderr, &error, "无法初始化触觉设备");
        fprintf(stderr, "\n按任意键退出.\n");
        getch();
        return -1;
    }

    printf("=== 触觉设备拖动坐标控制程序 ===\n");
    printf("发现设备型号: %s\n\n", hdGetString(HD_DEVICE_MODEL_TYPE));

    // 尝试加载配置文件
    g_dragController.loadFromFile("config.json");
    
    printInstructions();

    // 调度主回调函数
    hCallback = hdScheduleAsynchronous(deviceCallback, nullptr, HD_MAX_SCHEDULER_PRIORITY);

    hdEnable(HD_FORCE_OUTPUT);
    hdStartScheduler();

    // 检查错误
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "无法启动调度器");
        fprintf(stderr, "\n按任意键退出.\n");
        return -1;
    }

    // 主循环 - 处理键盘输入
    while (g_applicationRunning)
    {
        if (_kbhit())
        {
            handleKeyboard();
        }
        
        // 短暂延时避免CPU占用过高
        #if defined(WIN32)
        Sleep(10);
        #else
        usleep(10000);
        #endif
    }

    // 清理工作
    hdStopScheduler();
    hdUnschedule(hCallback);
    hdDisableDevice(hHD);

    printf("\n程序已退出.\n");
    return 0;
}

/*******************************************************************************
 设备回调函数 - 在触觉线程中执行
*******************************************************************************/
HDCallbackCode HDCALLBACK deviceCallback(void *data)
{
    HDErrorInfo error;
    hduVector3Dd position;
    int buttons;
    static int lastButtons = 0;

    HHD hHD = hdGetCurrentDevice();

    // 开始触觉帧
    hdBeginFrame(hHD);

    // 获取当前位置
    hdGetDoublev(HD_CURRENT_POSITION, position);
    
    // 获取当前变换矩阵（包含姿态信息）
    HDdouble transform[16];
    hdGetDoublev(HD_CURRENT_TRANSFORM, transform);
    
    // 获取按钮状态
    hdGetIntegerv(HD_CURRENT_BUTTONS, &buttons);

    // 转换为std::array格式
    std::array<double,3> pos = {position[0], position[1], position[2]};
    std::array<double,16> transformArray;
    for (int i = 0; i < 16; i++) {
        transformArray[i] = transform[i];
    }

    // 不需要速度计算，弹簧力只依赖位置

    // 检测按钮1状态（专门用于拖动）
    bool button1Pressed = (buttons & HD_DEVICE_BUTTON_1) != 0;
    static bool lastButton1Pressed = false;
    
    // 检测按钮2状态（专门用于夹抓）
    bool button2Pressed = (buttons & HD_DEVICE_BUTTON_2) != 0;
    static bool lastButton2Pressed = false;
    
    // 添加调试输出（检测到按钮变化时输出）
    if (button1Pressed != lastButton1Pressed) {
        printf("按钮1状态变化: buttons=0x%x, button1Pressed=%d\n", 
               buttons, button1Pressed ? 1 : 0);
    }
    if (button2Pressed != lastButton2Pressed) {
        printf("按钮2状态变化: buttons=0x%x, button2Pressed=%d\n", 
               buttons, button2Pressed ? 1 : 0);
    }
    
    // 按钮1控制拖动 - 按住拖动，松开释放
    if (button1Pressed != lastButton1Pressed) {
        if (button1Pressed) {
            // 按钮1按下：开始拖动
            g_dragController.onButtonDown(pos, transformArray);
            printf("按钮1按下：开始拖动\n");
        } else {
            // 按钮1松开：停止拖动
            g_dragController.onButtonUp();
            printf("按钮1松开：停止拖动\n");
        }
    }
    
    // 按钮2控制夹抓
    if (button2Pressed && !lastButton2Pressed) {
        // 按钮2按下：切换夹抓状态
        if (!g_isGripping) {
            g_isGripping = true;
            printf("按钮2按下：开始夹抓\n");
        } else {
            g_isGripping = false;
            printf("按钮2按下：松开夹抓\n");
        }
    }
    
    lastButton1Pressed = button1Pressed;
    lastButton2Pressed = button2Pressed;
    
        lastButtons = buttons;

    // 更新拖动状态
    g_dragController.update(pos, transformArray);

    // 应用弹簧力反馈（完全按照SimpleHapticScene的方式）
    hduVector3Dd force = {0.0, 0.0, 0.0};
    
    if (g_dragController.isDragging()) {
        std::array<double,3> springForce = g_dragController.calculateSpringForce(pos);
        force[0] = springForce[0];
        force[1] = springForce[1];
        force[2] = springForce[2];
        
        // 使用与SimpleHapticScene相同的力限制方法
        HDdouble forceClamp;
        hdGetDoublev(HD_NOMINAL_MAX_CONTINUOUS_FORCE, &forceClamp);
        
        // 计算力的大小
        double forceMagnitude = sqrt(force[0]*force[0] + force[1]*force[1] + force[2]*force[2]);
        
        // 与SimpleHapticScene相同的力限制逻辑
        if (forceMagnitude > forceClamp) {
            // 归一化并缩放到最大允许力
            if (forceMagnitude > 0.0) {
                double scale = forceClamp / forceMagnitude;
                force[0] *= scale;
                force[1] *= scale;
                force[2] *= scale;
            }
        }
    }
    
    hdSetDoublev(HD_CURRENT_FORCE, force);

    // 结束触觉帧
    hdEndFrame(hHD);

    // 检查错误
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "设备回调函数检测到错误\n");
        
        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;
        }
    }

    return HD_CALLBACK_CONTINUE;
}

/*******************************************************************************
 处理键盘输入
*******************************************************************************/
void handleKeyboard()
{
    int key = getch();
    
    switch (key) {
        case 'q':
        case 'Q':
            g_applicationRunning = false;
            break;
            
        case 's':
        case 'S':
            g_dragController.saveToFile("config.json");
            break;
            
        case 'l':
        case 'L':
            g_dragController.loadFromFile("config.json");
            break;
            
        case '+':
        case '=':
            g_dragController.setScale(g_dragController.getScale() * 1.1);
            break;
            
        case '-':
        case '_':
            g_dragController.setScale(g_dragController.getScale() * 0.9);
            break;
            
        case '[':
            g_dragController.setSpringStiffness(g_dragController.getSpringStiffness() * 0.9);
            break;
            
        case ']':
            g_dragController.setSpringStiffness(g_dragController.getSpringStiffness() * 1.1);
            break;
            
        case 'h':
        case 'H':
            printInstructions();
            break;
            
        case '1':
            g_selectedButton = HD_DEVICE_BUTTON_1;
            printf("已切换到按钮1\n");
            break;
            
        case '2':
            g_selectedButton = HD_DEVICE_BUTTON_2;
            printf("已切换到按钮2\n");
            break;
            
        case 'r':
        case 'R':
            if (g_isGripping) {
                g_dragController.onButtonUp();
                g_isGripping = false;
                printf("手动松开夹抓\n");
            }
            break;
            
        default:
            break;
    }
}

/*******************************************************************************
 打印使用说明
*******************************************************************************/
void printInstructions()
{
    printf("\n=== 操作说明 ===\n");
    printf("触觉设备操作:\n");
    printf("  - 按钮1: 按住拖动（弹簧力反馈），松开释放\n");
    printf("  - 按钮2: 切换夹抓状态（开始/停止夹抓）\n");
    printf("  - 移动/旋转设备: 感受向锚点拉扯的弹簧力，输出位置和姿态\n\n");
    
    printf("键盘操作:\n");
    printf("  s/S - 保存当前配置\n");
    printf("  l/L - 加载配置文件\n");
    printf("  +/= - 增加映射系数 (×1.1)\n");
    printf("  -/_ - 减少映射系数 (×0.9)\n");
    printf("  [   - 减少弹簧刚度 (×0.9)\n");
    printf("  ]   - 增加弹簧刚度 (×1.1)\n");
    printf("  1   - 切换到按钮1\n");
    printf("  2   - 切换到按钮2\n");
    printf("  r/R - 手动松开夹抓\n");
    printf("  h/H - 显示帮助\n");
    printf("  q/Q - 退出程序\n\n");
    
    printf("当前映射系数: %.3f\n", g_dragController.getScale());
    printf("当前弹簧刚度: %.3f\n", g_dragController.getSpringStiffness());
    printf("夹抓状态: %s\n", g_isGripping ? "闭合" : "松开");
    printf("拖动状态: %s\n", g_dragController.isDragging() ? "活动" : "非活动");
    printf("当前按钮: %s\n", g_selectedButton == HD_DEVICE_BUTTON_1 ? "按钮1" : "按钮2");
    printf("输出文件: drag_output.json\n");
    printf("配置文件: config.json\n\n");
    
    printf("准备就绪 - 按住按钮1拖动（弹簧力），按钮2控制夹抓...\n");
} 