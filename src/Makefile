#定义 路径等全局变量
export TOP_DIR:=$(shell pwd)
BIN_DIR:=$(TOP_DIR)/../bin
TARGET:=app_bin
BIN_TARGET:=$(BIN_DIR)/$(TARGET)
SRC_DIR:=$(shell ls -F | grep /$)  		


#定义函数 进入到每个子目录执行子目录的 makefile
define build_obj
for SubDir in $(SRC_DIR); \
do \
	if ! [ -d $$SubDir ]; then \
		echo "The $$SubDir is not exist !" ; \
			exit 11; \
	fi; \
	echo "Building $$SubDir ..."; \
	make -C $$SubDir; \
	if [ $$? -ne 0 ]; then \
		echo "Building $$SubDir is failed!"; \
		exit 12 ; \
	fi;	\
done
endef

#主目标
all:
	@$(call build_obj)
	gcc $(TOP_DIR)/../obj/*.o -lpthread -g -o $(BIN_TARGET) $(G_CFLAGS) $(INCLUDE_DIR)  $(G_LDFLAGS)

#清楚
.PHONY:clean
clean:
	-rm -f $(TOP_DIR)/../obj/*.o $(BIN_TARGET)
