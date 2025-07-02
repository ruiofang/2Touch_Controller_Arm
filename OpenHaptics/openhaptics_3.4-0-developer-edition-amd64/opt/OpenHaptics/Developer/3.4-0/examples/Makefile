CXX=g++
CXXFLAGS+=-W -O2 -DNDEBUG -Dlinux -std=c++11
LIBS = -lHDU -lHD -lrt -lncursesw -lstdc++ -lm

TARGET=DragControllerMain
HDRS=DragController.h conio.h
SRCS=DragControllerMain.cpp DragController.cpp conio.c
OBJS=$(SRCS:.cpp=.o)
OBJS:=$(OBJS:.c=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET) drag_output.json config.json

.PHONY: install
install: all
	@echo "程序编译完成: $(TARGET)"
	@echo "运行: ./$(TARGET)"

.PHONY: help
help:
	@echo "可用目标:"
	@echo "  all     - 编译程序"
	@echo "  clean   - 清理生成文件"
	@echo "  install - 编译并显示运行说明"
	@echo "  help    - 显示此帮助信息" 