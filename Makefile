# 双Touch设备双机械臂控制系统 Makefile
# 版本: 2.0.0

# 编译器设置
CXX = g++
CC = gcc
CXXFLAGS = -std=c++11 -Wall -Wextra -O3
CFLAGS = -Wall -Wextra -O3
LDFLAGS = 

# OpenHaptics 路径设置
OPENHAPTICS_ROOT = ./OpenHaptics/openhaptics_3.4-0-developer-edition-amd64
OPENHAPTICS_INCLUDE = $(OPENHAPTICS_ROOT)/usr/include
OPENHAPTICS_LIB = $(OPENHAPTICS_ROOT)/usr/lib

# 包含目录
INCLUDES = -I$(OPENHAPTICS_INCLUDE)

# 库文件
LIBS = -L$(OPENHAPTICS_LIB) -lHD -lHDU -lrt -lpthread -lncurses

# 源文件
SOURCES = Touch_Controller_Arm2.cpp conio.c
OBJECTS = Touch_Controller_Arm2.o conio.o
TARGET = Touch_Controller_Arm2

# 配置文件
CONFIG_FILE = config.ini

# 文档文件
DOCS = 双设备控制说明.md 双Touch设备使用说明.md 加载说明.md

# 测试目标
TEST_TARGET = test_device_config
TEST_OBJECTS = test_device_config.o conio.o

# 默认目标
.PHONY: all clean run help check install package debug release test-devices check-config

all: $(TARGET)

# 编译主程序
$(TARGET): $(OBJECTS)
	@echo "🔗 链接程序: $(TARGET)"
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS) $(LDFLAGS)
	@echo "✅ 编译完成: $(TARGET)"

# 编译设备配置测试程序
$(TEST_TARGET): $(TEST_OBJECTS)
	@echo "🔗 链接测试程序: $(TEST_TARGET)"
	$(CXX) $(TEST_OBJECTS) -o $(TEST_TARGET) $(LIBS)
	@echo "✅ 测试程序编译完成: $(TEST_TARGET)"

# 编译C++源文件
Touch_Controller_Arm2.o: Touch_Controller_Arm2.cpp ConfigLoader.h
	@echo "🔨 编译: Touch_Controller_Arm2.cpp"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c Touch_Controller_Arm2.cpp -o Touch_Controller_Arm2.o

# 编译C源文件
conio.o: conio.c conio.h
	@echo "🔨 编译: conio.c"
	$(CC) $(CFLAGS) -c conio.c -o conio.o

# 调试版本
debug: CXXFLAGS = -std=c++11 -Wall -Wextra -g -O0 -DDEBUG
debug: CFLAGS = -Wall -Wextra -g -O0 -DDEBUG
debug: clean $(TARGET)
	@echo "🐛 调试版本编译完成"

# 发布版本
release: CXXFLAGS = -std=c++11 -Wall -Wextra -O3 -DNDEBUG
release: CFLAGS = -Wall -Wextra -O3 -DNDEBUG
release: clean $(TARGET)
	@echo "🚀 发布版本编译完成"

# 运行程序
run: $(TARGET)
	@echo "🚀 启动双设备机械臂控制程序..."
	./$(TARGET)

# 过滤hEvent调试信息
run-clean: $(TARGET)
	@echo "🚀 启动双设备机械臂控制程序（过滤hEvent信息）..."
	@echo "注意: 仅过滤 'hEvent is NULL' 信息，其他日志正常显示"
	./$(TARGET) 2>&1 | grep -v "hEvent is NULL"

# 检查配置
check:
	@echo "=== 环境检查 ==="
	@echo "编译器版本:"
	@$(CXX) --version | head -1
	@echo ""
	@echo "OpenHaptics路径检查:"
	@if [ -d "$(OPENHAPTICS_ROOT)" ]; then \
		echo "✅ OpenHaptics目录存在: $(OPENHAPTICS_ROOT)"; \
	else \
		echo "❌ OpenHaptics目录不存在: $(OPENHAPTICS_ROOT)"; \
		exit 1; \
	fi
	@echo ""
	@echo "库文件检查:"
	@if [ -f "$(OPENHAPTICS_LIB)/libHD.so.3.4.0" ]; then \
		echo "✅ HD库存在"; \
	else \
		echo "❌ HD库不存在"; \
	fi
	@if [ -f "$(OPENHAPTICS_LIB)/libHDU.a" ]; then \
		echo "✅ HDU库存在"; \
	else \
		echo "❌ HDU库不存在"; \
	fi
	@echo ""
	@echo "配置文件检查:"
	@if [ -f "$(CONFIG_FILE)" ]; then \
		echo "✅ 配置文件存在: $(CONFIG_FILE)"; \
	else \
		echo "⚠️  配置文件不存在，将使用默认配置"; \
	fi
	@echo ""
	@echo "系统库检查:"
	@pkg-config --exists ncurses && echo "✅ ncurses库可用" || echo "❌ ncurses库不可用"
	@echo "=================="

# 安装程序
install: $(TARGET)
	@echo "📦 安装程序..."
	sudo mkdir -p /usr/local/bin
	sudo cp $(TARGET) /usr/local/bin/
	sudo mkdir -p /etc/dual-touch-arm-controller
	sudo cp $(CONFIG_FILE) /etc/dual-touch-arm-controller/
	sudo mkdir -p /usr/local/share/doc/dual-touch-arm-controller
	sudo cp $(DOCS) /usr/local/share/doc/dual-touch-arm-controller/
	@echo "✅ 安装完成"
	@echo "   程序位置: /usr/local/bin/$(TARGET)"
	@echo "   配置文件: /etc/dual-touch-arm-controller/$(CONFIG_FILE)"
	@echo "   文档位置: /usr/local/share/doc/dual-touch-arm-controller/"

# 卸载程序
uninstall:
	@echo "🗑️  卸载程序..."
	sudo rm -f /usr/local/bin/$(TARGET)
	sudo rm -rf /etc/dual-touch-arm-controller
	sudo rm -rf /usr/local/share/doc/dual-touch-arm-controller
	@echo "✅ 卸载完成"

# 创建软件包
package: release
	@echo "📦 创建软件包..."
	mkdir -p dual-touch-arm-controller-2.0.0
	cp $(TARGET) dual-touch-arm-controller-2.0.0/
	cp $(CONFIG_FILE) dual-touch-arm-controller-2.0.0/
	cp $(DOCS) dual-touch-arm-controller-2.0.0/
	cp 启动双机械臂控制.sh dual-touch-arm-controller-2.0.0/ 2>/dev/null || true
	cp README.md dual-touch-arm-controller-2.0.0/ 2>/dev/null || true
	tar -czf dual-touch-arm-controller-2.0.0.tar.gz dual-touch-arm-controller-2.0.0/
	rm -rf dual-touch-arm-controller-2.0.0/
	@echo "✅ 软件包创建完成: dual-touch-arm-controller-2.0.0.tar.gz"

# 测试编译
test-compile: clean
	@echo "🧪 测试编译..."
	$(MAKE) all
	@echo "✅ 测试编译成功"

# 代码格式化 (如果有clang-format)
format:
	@echo "🎨 格式化代码..."
	@if command -v clang-format >/dev/null 2>&1; then \
		clang-format -i Touch_Controller_Arm2.cpp; \
		echo "✅ 代码格式化完成"; \
	else \
		echo "⚠️  clang-format未安装，跳过格式化"; \
	fi

# 清理生成文件
clean:
	@echo "🧹 清理生成文件..."
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGET) *.o
	rm -f dual-touch-arm-controller-*.tar.gz
	rm -rf dual-touch-arm-controller-*/
	@echo "✅ 清理完成"

# 显示帮助信息
help:
	@echo "=== 双Touch设备双机械臂控制系统 Makefile 帮助 ==="
	@echo ""
	@echo "可用目标:"
	@echo "  all          - 编译程序 (默认)"
	@echo "  debug        - 编译调试版本"
	@echo "  release      - 编译发布版本"
	@echo "  run          - 运行程序 (完整日志)"
	@echo "  run-clean    - 运行程序 (仅过滤hEvent信息)"
	@echo "  check        - 检查编译环境"
	@echo "  test-compile - 测试编译"
	@echo "  clean        - 清理生成文件"
	@echo "  install      - 安装程序到系统"
	@echo "  uninstall    - 从系统卸载程序"
	@echo "  package      - 创建软件包"
	@echo "  format       - 格式化代码"
	@echo "  help         - 显示此帮助信息"
	@echo "  test-devices - 运行设备配置测试"
	@echo "  check-config - 检查配置文件"
	@echo ""
	@echo "使用示例:"
	@echo "  make         # 编译程序"
	@echo "  make run     # 编译并运行程序"
	@echo "  make debug   # 编译调试版本"
	@echo "  make check   # 检查环境"
	@echo "  make clean   # 清理文件"
	@echo "  sudo make install  # 安装到系统"
	@echo ""
	@echo "配置:"
	@echo "  OpenHaptics路径: $(OPENHAPTICS_ROOT)"
	@echo "  包含目录: $(OPENHAPTICS_INCLUDE)"
	@echo "  库目录: $(OPENHAPTICS_LIB)"
	@echo "  编译器: $(CXX)"
	@echo "  C++标准: C++11"
	@echo "  程序文件: $(TARGET)"
	@echo "=================================================="

# 依赖关系
Touch_Controller_Arm2.cpp: ConfigLoader.h
conio.c: conio.h

# 运行设备配置测试
test-devices: $(TEST_TARGET)
	@echo "🧪 启动设备配置测试程序..."
	./$(TEST_TARGET)

# 检查配置文件
check-config:
	@if [ ! -f "config.ini" ]; then \
		echo "⚠️  配置文件不存在，将使用默认配置"; \
	else \
		echo "✅ 配置文件存在: config.ini"; \
		echo "📄 当前配置内容:"; \
		cat config.ini; \
	fi 