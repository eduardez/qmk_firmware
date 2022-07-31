# MCU name
MCU = RP2040

# Bootloader selection
BOOTLOADER = rp2040

# Board selection
BOARD = GENERIC_RP_RP2040

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = no       # Enable Bootmagic Lite
MOUSEKEY_ENABLE = no       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = yes        # Console for debug
COMMAND_ENABLE = yes        # Commands for debug and configuration
NKRO_ENABLE = no            # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output

# DEBOUNCE_TYPE = sym_eager_pr


CUSTOM_MATRIX = lite        # I2C PCF8574 and PCF8575
SRC += matrix.c
QUANTUM_LIB_SRC += i2c_master.c