## Makefile
include $(TOP_DIR)/project.mk
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
.PHONY:all
all:$(OBJS)
