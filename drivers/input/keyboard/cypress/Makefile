#
# Makefile for the Cypress touchkey driver
#
ifeq ($(CONFIG_CPU_EXYNOS4210),y)
obj-$(CONFIG_KEYBOARD_CYPRESS_TOUCH) += cypress-touchkey_4210.o issp_driver_routines.o issp_main.o issp_routines.o
else
obj-$(CONFIG_KEYBOARD_CYPRESS_TOUCH) += cypress-touchkey.o issp_driver_routines.o issp_main.o issp_routines.o
endif
