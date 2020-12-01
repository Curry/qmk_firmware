BOOTLOADER              = atmel-dfu
SPLIT_TRANSPORT         = custom

QUANTUM_LIB_SRC += serial.c i2c_master.c i2c_slave.c
SRC += custom_transport.c
