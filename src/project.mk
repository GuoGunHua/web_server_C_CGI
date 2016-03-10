CC := gcc

src_dir:=$(shell ls -F $(TOP_DIR) | grep /$)
export INCLUDE_DIR :=  $(foreach dir,$(src_dir),-I$(TOP_DIR)/$(dir)include)	


G_CFLAGS :=
G_LDFLAGS :=

%.o : %.c
	$(CC)  -c	-g $< -o $(TOP_DIR)/../obj/$@ $(G_CFLAGS) $(INCLUDE_DIR)  $(G_LDFLAGS)
