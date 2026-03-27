KERNEL_DEV_DIR := /usr/src/kernels/$(shell uname -r)
KERNEL_MODULE_OBJ_LIST :=
MODULE_PRIVATE_KEY := 
MODULE_PUBLIC_KEY := 
SIGN_FILE := $(KERNEL_DEV_DIR)/scripts/sign-file

obj-m += $(KERNEL_MODULE_OBJ_LIST)

MODULE_KO := $(patsubst %.o,%.ko,$(KERNEL_MODULE_OBJ_LIST))

all: make_help

build:
	make -C $(KERNEL_DEV_DIR) M=$(PWD) modules
	for m in $(MODULE_KO); \
	do \
		$(SIGN_FILE) sha256 $(MODULE_PRIVATE_KEY) $(MODULE_PUBLIC_KEY) $$m; \
	done

make_help:
	@echo "External variables : "
	@echo "  KERNEL_MODULE_OBJ_LIST - name of module object file"
	@echo "  # need to specify kernel module object file to start build"
	@echo "Targets : "
	@echo "  build - build module"
	@echo "  help - show kernel Makefile help"
	@echo "  make_help - show this help"
	@echo "  clean - remove object files"

help:
	make -C $(KERNEL_DEV_DIR) M=$(PWD) help

.PHONY: clean
clean:
	make -C $(KERNEL_DEV_DIR) M=$(PWD) clean
	@rm -f *.order
	@rm -f *.symvers
	@rm -f *.mod.*



