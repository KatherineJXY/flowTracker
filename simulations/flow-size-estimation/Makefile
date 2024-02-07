CC = g++ # 使用的编译器
CFLAGS = -O2 -std=c++14 -Wall # 编译选项
LDFLAGS = # 链接选项
SRC_DIR = . # 源文件目录
UTILS_DIR = utils # 实用工具文件目录
BUILD_DIR = build # 编译后文件存放目录
TARGET = flow-size-estimation # 最终生成的可执行文件名

# 所有的源文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(UTILS_DIR)/*.cpp)
# 所有的目标文件
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# 默认目标：编译可执行文件
all: $(BUILD_DIR) $(TARGET)

# 创建编译后文件存放目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# 编译每个源文件为目标文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -I$(UTILS_DIR) -c $< -o $@

# 清理编译生成的文件
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# 声明伪目标，防止与同名文件冲突
.PHONY: all clean
