# 使用的编译器
CC = g++
# 编译选项
CFLAGS = -O2 -std=c++14 -Wall
# 链接选项
LDFLAGS =
# 源文件目录
SRC_DIR = .
# 实用工具文件目录
UTILS_DIR = ../utils
# 编译后文件存放目录
BUILD_DIR = build
# 最终生成的可执行文件名
TARGET = average-delay-estimation

# 所有的源文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(UTILS_DIR)/*.cpp)
# 所有的目标文件
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# 默认目标：编译可执行文件
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 创建编译后文件存放目录
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# 编译每个源文件为目标文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(UTILS_DIR) -c $< -o $@

# 清理编译生成的文件
clean:
	rmdir /s /q $(BUILD_DIR) 2>nul
	del $(TARGET).exe 2>nul

# 声明伪目标，防止与同名文件冲突
.PHONY: all clean
