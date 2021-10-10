PROJECT_NAME     := etna
TARGETS          := app
OUTPUT_DIRECTORY := build
GCC_VERSION := ${GCC_PATH}/gcc-arm-none-eabi-10-2020-q4-major

SDK_ROOT := ${NRF_PATH}/v17.0
PROJ_DIR := ./
SD_HEX := $(SDK_ROOT)/components/softdevice/s113/hex/s113_nrf52_7.2.0_softdevice.hex
BRANCH := $(shell git rev-parse --abbrev-ref HEAD)
HASH := $(shell git rev-parse --short HEAD)
USB_PD_SRC := $(PROJ_DIR)/src/components/usb_pd

$(OUTPUT_DIRECTORY)/$(TARGETS).out: \
  LINKER_SCRIPT  := nrf52833xxAA.ld

# Source files common to all targets
SRC_FILES += \
	$(PROJ_DIR)/src/main.c \
	$(PROJ_DIR)/src/etna.c \
	$(PROJ_DIR)/src/hal_nrf52.c \
	$(PROJ_DIR)/src/components/ssd1309/ssd1309.c  \
	$(PROJ_DIR)/src/components/gfx/gfx.c  \
	$(PROJ_DIR)/src/components/gfx/fonts.c  \
	$(PROJ_DIR)/src/components/ads1118/ads1118.c  \
	$(wildcard $(USB_PD_SRC)/*.c) \
	$(wildcard $(USB_PD_SRC)/platform/*.c) \
	$(wildcard $(USB_PD_SRC)/modules/*.c) \
	$(wildcard $(USB_PD_SRC)/vdm/*.c) \
	$(wildcard $(USB_PD_SRC)/vdm/DisplayPort/*.c) \
	$(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52833.S \
	$(SDK_ROOT)/modules/nrfx/mdk/system_nrf52833.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_rng.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_rng.c \
	$(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
	$(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
	$(SDK_ROOT)/components/boards/boards.c \
	$(SDK_ROOT)/components/libraries/button/app_button.c \
	$(SDK_ROOT)/components/libraries/util/app_error.c \
	$(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
	$(SDK_ROOT)/components/libraries/util/app_error_weak.c \
	$(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  	$(SDK_ROOT)/components/libraries/timer/app_timer2.c \
  	$(SDK_ROOT)/components/libraries/timer/drv_rtc.c \
  	$(SDK_ROOT)/components/libraries/sortlist/nrf_sortlist.c \
	$(SDK_ROOT)/components/libraries/pwm/app_pwm.c \
	$(SDK_ROOT)/components/libraries/util/app_util_platform.c \
	$(SDK_ROOT)/components/libraries/crc16/crc16.c \
	$(SDK_ROOT)/components/libraries/fds/fds.c \
	$(SDK_ROOT)/components/libraries/util/nrf_assert.c \
	$(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
	$(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
	$(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
	$(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
	$(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
	$(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
	$(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
	$(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
	$(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
	$(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
	$(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
	$(SDK_ROOT)/components/ble/peer_manager/auth_status_tracker.c \
	$(SDK_ROOT)/components/ble/peer_manager/nrf_ble_lesc.c \
	$(SDK_ROOT)/components/ble/common/ble_advdata.c \
	$(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
	$(SDK_ROOT)/components/ble/common/ble_conn_params.c \
	$(SDK_ROOT)/components/ble/common/ble_conn_state.c \
	$(SDK_ROOT)/components/ble/common/ble_srv_common.c \
	$(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
	$(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
	$(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
	$(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
	$(SDK_ROOT)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_manager_handler.c \
	$(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
	$(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
	$(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
	$(SDK_ROOT)/components/ble/ble_services/ble_dfu/ble_dfu.c \
	$(SDK_ROOT)/components/ble/ble_services/ble_dfu/ble_dfu_bonded.c \
	$(SDK_ROOT)/components/ble/ble_services/ble_dfu/ble_dfu_unbonded.c \
	$(SDK_ROOT)/components/ble/ble_services/ble_bas/ble_bas.c \
	$(SDK_ROOT)/components/ble/ble_services/ble_dis/ble_dis.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_svci.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c \
	$(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uart.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twim.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twi_twim.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spim.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spi.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_ppi.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_timer.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_saadc.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_wdt.c \
	$(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
	$(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
	$(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
	$(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
	$(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_ppi.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aead.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aes.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aes_shared.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecc.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdh.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdsa.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_eddsa.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_error.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hash.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hkdf.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hmac.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_init.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_rng.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_shared.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_init.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng_mbedtls.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_eddsa.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hash.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng_mbedtls.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng.c \
	$(SDK_ROOT)/external/mbedtls/library/aes.c \
  	$(SDK_ROOT)/external/mbedtls/library/ctr_drbg.c \
  	$(SDK_ROOT)/external/mbedtls/library/platform_util.c \
	$(SDK_ROOT)/components/ble/ble_dtm/ble_dtm.c \
  	$(SDK_ROOT)/components/ble/ble_dtm/ble_dtm_hw_nrf52.c \
  	$(SDK_ROOT)/components/libraries/usbd/app_usbd.c \
  	$(SDK_ROOT)/components/libraries/usbd/class/cdc/acm/app_usbd_cdc_acm.c \
  	$(SDK_ROOT)/components/libraries/usbd/app_usbd_core.c \
  	$(SDK_ROOT)/components/libraries/usbd/app_usbd_serial_num.c \
  	$(SDK_ROOT)/components/libraries/usbd/app_usbd_string_desc.c \
  	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_usbd.c \
  	$(SDK_ROOT)/external/utf_converter/utf.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_power.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_power.c \
	


# Include folders common to all targets
INC_FOLDERS += \
	$(PROJ_DIR)/src/ \
	$(PROJ_DIR)/src/config \
	$(PROJ_DIR)/src/components/ads1118\
  	$(PROJ_DIR)/src/components/gfx\
  	$(PROJ_DIR)/src/components/ssd1309\
	$(PROJ_DIR)/src/components/usb_pd\
	$(PROJ_DIR)/src/components/usb_pd/modules\
	$(PROJ_DIR)/src/components/usb_pd/vdm\
	$(SDK_ROOT)/components/toolchain/cmsis/include \
	$(SDK_ROOT)/components/toolchain/cmsis/dsp/Include \
	$(PROJ_DIR)/src/components/usb_pd/vdm/DisplayPort\
	$(PROJ_DIR)/src/components/usb_pd/platform\
	$(SDK_ROOT)/components \
	$(SDK_ROOT)/modules/nrfx/mdk \
	$(SDK_ROOT)/components/libraries/scheduler \
	$(SDK_ROOT)/modules/nrfx \
	$(SDK_ROOT)/components/libraries/pwr_mgmt \
	$(SDK_ROOT)/components/libraries/strerror \
	$(SDK_ROOT)/components/softdevice/common \
	$(SDK_ROOT)/components/softdevice/s113/headers/nrf52 \
	$(SDK_ROOT)/components/softdevice/s113/headers \
	$(SDK_ROOT)/components/libraries/crc16 \
	$(SDK_ROOT)/components/libraries/bootloader/dfu \
	$(SDK_ROOT)/components/libraries/util \
	$(SDK_ROOT)/components/ble/common \
	$(SDK_ROOT)/components/libraries/balloc \
	$(SDK_ROOT)/components/ble/peer_manager \
	$(SDK_ROOT)/components/libraries/ringbuf \
	$(SDK_ROOT)/components/libraries/bsp \
	$(SDK_ROOT)/components/libraries/timer \
	$(SDK_ROOT)/components/libraries/pwm \
	$(SDK_ROOT)/components/libraries/log \
	$(SDK_ROOT)/components/ble/nrf_ble_gatt \
	$(SDK_ROOT)/components/ble/nrf_ble_qwr \
	$(SDK_ROOT)/components/libraries/button \
	$(SDK_ROOT)/components/libraries/bootloader \
	$(SDK_ROOT)/components/libraries/fstorage \
	$(SDK_ROOT)/components/libraries/experimental_section_vars \
	$(SDK_ROOT)/components/libraries/mutex \
	$(SDK_ROOT)/components/libraries/delay \
	$(SDK_ROOT)/components/libraries/bootloader/ble_dfu \
	$(SDK_ROOT)/components/libraries/atomic_fifo \
	$(SDK_ROOT)/components/libraries/atomic \
	$(SDK_ROOT)/components/boards \
	$(SDK_ROOT)/components/libraries/memobj \
	$(SDK_ROOT)/components/libraries/fds \
	$(SDK_ROOT)/components/ble/ble_advertising \
	$(SDK_ROOT)/components/libraries/queue/ \
	$(SDK_ROOT)/components/libraries/atomic_flags \
	$(SDK_ROOT)/components/libraries/svc \
	$(SDK_ROOT)/components/libraries/log/src \
	$(SDK_ROOT)/components/ble/ble_services/ble_dfu \
	$(SDK_ROOT)/components/ble/ble_services/ble_dis \
	$(SDK_ROOT)/components/ble/ble_services/ble_bas \
	$(SDK_ROOT)/components/libraries/crypto \
	$(SDK_ROOT)/components/libraries/queue \
	$(SDK_ROOT)/components/libraries/crypto/backend/cc310 \
	$(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl \
	$(SDK_ROOT)/external/nrf_tls/mbedtls/nrf_crypto/config \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon \
	$(SDK_ROOT)/components/libraries/crypto/backend/mbedtls \
	$(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc \
	$(SDK_ROOT)/components/libraries/crypto/backend/optiga \
	$(SDK_ROOT)/components/libraries/crypto/backend/cifra \
	$(SDK_ROOT)/external/nrf_oberon \
	$(SDK_ROOT)/external/nrf_oberon/include \
	$(SDK_ROOT)/external/mbedtls/include \
	$(SDK_ROOT)/external/mbedtls/include/mbedtls \
	$(SDK_ROOT)/external/nrf_cc310/include \
	$(SDK_ROOT)/modules/nrfx/hal \
	$(SDK_ROOT)/modules/nrfx/drivers/include \
	$(SDK_ROOT)/external/segger_rtt \
	$(SDK_ROOT)/external/fprintf \
	$(SDK_ROOT)/integration/nrfx/legacy \
	$(SDK_ROOT)/integration/nrfx \
	$(SDK_ROOT)/external/nrf_tls/mbedtls/nrf_crypto/config \
	$(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw \
	$(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw \
	$(SDK_ROOT)/components/libraries/stack_info \
	$(SDK_ROOT)/components/ble/ble_dtm \
	$(SDK_ROOT)/components/libraries/sortlist \
	$(SDK_ROOT)/components/libraries/uart \
	$(SDK_ROOT)/components/libraries/usbd \
	$(SDK_ROOT)/components/libraries/usbd/class/cdc \
	$(SDK_ROOT)/components/libraries/usbd/class/cdc/acm \
	$(SDK_ROOT)/components/ble/ble_services/ble_nus \
	$(SDK_ROOT)/components/ble/ble_link_ctx_manager \
	$(SDK_ROOT)/external/utf_converter \


# Libraries common to all targets
# Libraries common to all targets
LIB_FILES += \
	$(SDK_ROOT)/external/nrf_cc310/lib/cortex-m4/hard-float/libnrf_cc310_0.9.13.a \
	$(SDK_ROOT)/external/nrf_oberon/lib/cortex-m4/hard-float/liboberon_3.0.6.a \
	$(SDK_ROOT)/components/toolchain/cmsis/dsp/GCC/libarm_cortexM4lf_math.a \

# Optimization flags
#OPT = -Os -g3
OPT = -O0 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DBL_SETTINGS_ACCESS_ONLY
CFLAGS += -DAPP_TIMER_V2
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS
CFLAGS += -DNRF52833_XXAA
CFLAGS += -DMBEDTLS_CONFIG_FILE=\"nrf_crypto_mbedtls_config.h\"
CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS
CFLAGS += -DNRF_CRYPTO_MAX_INSTANCE_COUNT=1
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DNRF_DFU_SVCI_ENABLED
CFLAGS += -DNRF_DFU_TRANSPORT_BLE=1
CFLAGS += -DNRF_SD_BLE_API_VERSION=7
CFLAGS += -DS113
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DPLATFORM_NONE
CFLAGS += -DARM_MATH_CM4
CFLAGS += -DFSC_HAVE_SNK 
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DAPP_TIMER_V2
ASMFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
ASMFLAGS += -DBL_SETTINGS_ACCESS_ONLY
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52833_XXAA
ASMFLAGS += -DNRF_DFU_SVCI_ENABLED
ASMFLAGS += -DNRF_DFU_TRANSPORT_BLE=1
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=7
ASMFLAGS += -DS113
ASMFLAGS += -DSOFTDEVICE_PRESENT

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -u _printf_float
# use newlib in nano version
LDFLAGS += --specs=nano.specs
# to allow floating values to be printed
LDFLAGS += -u _printf_float

$(TARGETS): CFLAGS += -D__HEAP_SIZE=2048
$(TARGETS): CFLAGS += -D__STACK_SIZE=2048
$(TARGETS): ASMFLAGS += -D__HEAP_SIZE=2048
$(TARGETS): ASMFLAGS += -D__STACK_SIZE=2048

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm



.PHONY: default help

# Default target - first one defined
default: $(TARGETS)
	rm -f $(OUTPUT_DIRECTORY)/$(TARGETS).bin

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		$(TARGETS)
	@echo		flash_sd
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash flash_sd erase 


# Flash the program
flash: default
	@echo Flashing Application via JLINK...
	nrfutil settings generate --family NRF52 --application $(OUTPUT_DIRECTORY)/$(TARGETS).hex --application-version 1 --bootloader-version 1 --bl-settings-version 1 $(OUTPUT_DIRECTORY)/bl_settings.hex
	mergehex -m $(OUTPUT_DIRECTORY)/$(TARGETS).hex $(OUTPUT_DIRECTORY)/bl_settings.hex -o $(OUTPUT_DIRECTORY)/$(PROJECT_NAME).hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/$(PROJECT_NAME).hex --sectorerase -c 4000
	nrfjprog -f nrf52 --reset 

release: default
	nrfutil settings generate --family NRF52 --application $(OUTPUT_DIRECTORY)/$(TARGETS).hex --application-version 1 --bootloader-version 1 --bl-settings-version 1 $(OUTPUT_DIRECTORY)/bls.hex
	mergehex -m $(OUTPUT_DIRECTORY)/$(TARGETS).hex $(OUTPUT_DIRECTORY)/bls.hex -o $(OUTPUT_DIRECTORY)/tmp.hex
	mkdir -p $(OUTPUT_DIRECTORY)/release
	mergehex -m $(SD_HEX) ../boot/build/boot.hex $(OUTPUT_DIRECTORY)/tmp.hex -o $(OUTPUT_DIRECTORY)/release/$(PROJECT_NAME)-fw-$(VERSION)-$(BRANCH)-$(HASH).hex
	rm $(OUTPUT_DIRECTORY)/bls.hex
	rm $(OUTPUT_DIRECTORY)/tmp.hex

flash_all: release
	@echo Flashing SD, Bootloader and Application via JLINK...
	nrfjprog -f nrf52 --eraseall
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/release/$(PROJECT_NAME)-fw-$(VERSION)-$(BRANCH)-$(HASH).hex -c 4000
	nrfjprog -f nrf52 --reset 

package: default
	@echo Creating DFU Image
	mkdir -p $(OUTPUT_DIRECTORY)/dfu
	nrfutil pkg generate --hw-version 52 --sd-req 0xCB --application-version 2 --application $(OUTPUT_DIRECTORY)/$(TARGETS).hex --key-file key.pem $(OUTPUT_DIRECTORY)/dfu/$(PROJECT_NAME)-dfu-$(VERSION)-$(BRANCH)-$(HASH).zip

dfu: package
	@echo Updating via DFU
	nrfutil dfu usb-serial -pkg $(OUTPUT_DIRECTORY)/dfu/$(PROJECT_NAME)-dfu-$(VERSION)-$(BRANCH)-$(HASH).zip -p /dev/ttyUSB0 -fc 0 -cd 0 -t 0

erase:
	nrfjprog -f nrf52 --eraseall

reset:
	nrfjprog -f nrf52 --reset

SDK_CONFIG_FILE := $(PROJ_DIR)/src/config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
