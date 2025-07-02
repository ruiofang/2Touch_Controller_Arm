# Makefile for USB-HID Reader
# 基于OpenHaptics SDK和libusb-1.0

# 编译器设置
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# OpenHaptics SDK路径
OPENHAPTICS_DIR = openhaptics_3.4-0-developer-edition-amd64/usr
INCLUDE_DIR = $(OPENHAPTICS_DIR)/include
LIB_DIR = $(OPENHAPTICS_DIR)/lib

# 包含路径
INCLUDES = -I$(INCLUDE_DIR) -I/usr/include/libusb-1.0

# 库文件
LIBS = -L$(LIB_DIR) -lHD -lHDU -lusb-1.0 -lpthread

# PhantomIOLib42路径 (如果存在)
PHANTOM_LIB = TouchDriver_2023_01_12/usr/lib/libPhantomIOLib42.so
ifneq (,$(wildcard $(PHANTOM_LIB)))
    LIBS += -L$(dir $(PHANTOM_LIB)) -lPhantomIOLib42
endif

# 目标文件
TARGET = usb_hid_reader_simple
SOURCE = usb_hid_reader_simple.cpp
TARGET_FULL = usb_hid_reader
SOURCE_FULL = usb_hid_reader.cpp

# 主要目标
all: $(TARGET)

# 简化版本 (仅使用libusb)
$(TARGET): $(SOURCE)
	@echo "编译简化版 $(TARGET)..."
	@echo "使用的库: -lusb-1.0 -lpthread"
	$(CXX) $(CXXFLAGS) -I/usr/include/libusb-1.0 -o $(TARGET) $(SOURCE) -lusb-1.0 -lpthread
	@echo "编译完成!"

# 完整版本 (包含OpenHaptics)
full: $(TARGET_FULL)

$(TARGET_FULL): $(SOURCE_FULL)
	@echo "编译完整版 $(TARGET_FULL)..."
	@echo "使用的包含路径: $(INCLUDES)"
	@echo "使用的库: $(LIBS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET_FULL) $(SOURCE_FULL) $(LIBS)
	@echo "编译完成!"

# 清理
clean:
	rm -f $(TARGET) $(TARGET_FULL)
	@echo "清理完成"

# 安装 (需要root权限)
install: $(TARGET)
	@echo "安装程序到 /usr/local/bin..."
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/$(TARGET)
	@echo "安装完成"

# 卸载
uninstall:
	@echo "从 /usr/local/bin 卸载..."
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "卸载完成"

# 运行 (需要适当权限)
run: $(TARGET)
	@echo "运行程序..."
	@echo "注意: 可能需要sudo权限来访问USB设备"
	./$(TARGET)

# 调试版本
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# 检查依赖
check-deps:
	@echo "检查依赖库..."
	@echo -n "检查 OpenHaptics headers: "
	@test -d $(INCLUDE_DIR)/HD && echo "✓ 找到" || echo "✗ 未找到"
	@echo -n "检查 OpenHaptics 库: "
	@test -f $(LIB_DIR)/libHD.so* && echo "✓ 找到" || echo "✗ 未找到"
	@echo -n "检查 libusb-1.0: "
	@pkg-config --exists libusb-1.0 && echo "✓ 找到" || echo "✗ 未找到"
	@echo -n "检查 PhantomIOLib42: "
	@test -f $(PHANTOM_LIB) && echo "✓ 找到" || echo "✗ 未找到 (可选)"

# 显示帮助
help:
	@echo "可用目标:"
	@echo "  all       - 编译简化版程序 (默认，仅libusb)"
	@echo "  full      - 编译完整版程序 (包含OpenHaptics)"
	@echo "  clean     - 清理编译文件"
	@echo "  debug     - 编译调试版本"
	@echo "  run       - 编译并运行程序"
	@echo "  install   - 安装到系统 (需要sudo)"
	@echo "  uninstall - 从系统卸载 (需要sudo)"
	@echo "  check-deps- 检查依赖库"
	@echo "  help      - 显示此帮助"

.PHONY: all clean install uninstall run debug check-deps help 