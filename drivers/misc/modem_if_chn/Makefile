# Makefile of modem_if

EXTRA_CFLAGS += -Idrivers/misc/modem_if

obj-y += sipc5_modem.o sipc5_io_device.o
obj-y += sipc4_modem.o sipc4_io_device.o
obj-y += modem_net_flowcontrol_device.o modem_utils.o

obj-$(CONFIG_CDMA_MODEM_MDM6600) += modem_modemctl_device_mdm6600.o
obj-$(CONFIG_GSM_MODEM_ESC6270) += modem_modemctl_device_esc6270.o

obj-$(CONFIG_LINK_DEVICE_MIPI) += modem_link_device_mipi.o
obj-$(CONFIG_LINK_DEVICE_DPRAM) += modem_link_device_dpram.o modem_link_device_dpram_ext_op.o
obj-$(CONFIG_LINK_DEVICE_USB) += modem_link_device_usb.o modem_link_pm_usb.o
obj-$(CONFIG_LINK_DEVICE_HSIC) += modem_link_device_hsic.o
