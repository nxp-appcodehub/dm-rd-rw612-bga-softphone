
# Copyright 2026 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.22.0)

cmake_path(APPEND conf_file_path ${CMAKE_CURRENT_LIST_DIR} ${board} prj.conf)
set(CONF_FILE ${conf_file_path} ${CONF_FILE})
include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(rdrw612bga_wifi_softphone LANGUAGES C CXX ASM PROJECT_BOARD_PORT_PATH ${board_root}/${board}/wifi_examples/wifi_ipv4_ipv6_echo)

include(
    sdk_patch/sdk_patch.cmake
)

include(${board}/board_files.cmake)
include(${SdkRootDirPath}/CMakeLists.txt)

add_compile_definitions(CONFIG_MONOLITHIC_WIFI=1)
add_compile_definitions(gPlatformMonolithicApp_d=1)
add_compile_definitions(CONFIG_SOC_SERIES_RW6XX_REVISION_A2=1)
add_compile_definitions(CONFIG_UNCOMPRESSED_FIRMWARE=1)

# Select CA certificate header
option(USE_CUSTOM_CA "Use custom CA certificate instead of the default one" OFF)
option(ENABLE_PRINT_CERTIFICATE_INFO "Enable TLS certificate information printing" OFF)
# UI mode selection: set to ON to build without the LVGL graphical UI (CLI Shell only)
option(DISABLE_LVGL_UI "Disable the LVGL graphical UI and build with the CLI Shell interface only" OFF)

if(DISABLE_LVGL_UI)
    message(STATUS "LVGL UI disabled -- building CLI Shell only (CONFIG_CLI_ONLY)")
    add_compile_definitions(CONFIG_CLI_ONLY=1)
else()
    message(STATUS "LVGL UI enabled (default)")
endif()

set(OPUS_BASE "${SdkRootDirPath}/middleware/audio_voice/components/opus")

file(GLOB OPUS_CELT_C         "${OPUS_BASE}/celt/*.c")
file(GLOB OPUS_SILK_C         "${OPUS_BASE}/silk/*.c")
file(GLOB OPUS_SILK_FIXED_C   "${OPUS_BASE}/silk/fixed/*.c")
file(GLOB OPUS_SRC_C          "${OPUS_BASE}/src/*.c")

set(OPUS_ALL_C
    ${OPUS_CELT_C}
    ${OPUS_SILK_C}
    ${OPUS_SILK_FIXED_C}
    ${OPUS_SRC_C}
)

foreach(src ${OPUS_ALL_C})
  set_source_files_properties("${src}" APPEND PROPERTIES
    COMPILE_FLAGS "-O2"
  )
endforeach()

mcux_add_include(
    INCLUDES ipv4_ipv6_echo_common
)

if(NOT DISABLE_LVGL_UI)
    mcux_add_include(
        INCLUDES custom
                 generated
                 generated/guider_customer_fonts
                 generated/guider_fonts
                 generated/images
                 lvgl_examples
    )

    mcux_add_source(
        SOURCES custom/custom.c
                custom/custom.h
                custom/lv_conf_ext.h
                generated/guider_fonts/lv_font_Antonio_Regular_12.c
                generated/guider_fonts/lv_font_Antonio_12.c
                generated/guider_fonts/lv_font_Antonio_Regular_16.c
                generated/guider_fonts/lv_font_montserratMedium_12.c
                generated/images/_wifi_RGB565A8_38x38.c
                generated/images/_call_RGB565A8_97x87.c
                generated/images/_settings_RGB565A8_60x40.c
                generated/images/_keypad_RGB565A8_115x97.c
                generated/images/_call_end_RGB565A8_85x85.c
                generated/images/_call_RGB565A8_85x85.c
                generated/images/_back_RGB565A8_55x40.c
                generated/images/_back_RGB565A8_35x40.c
                generated/images/_send_RGB565A8_65x55.c
                generated/images/_chat_RGB565A8_65x55.c
                generated/images/_chat_RGB565A8_100x50.c
                generated/gui_guider.c
                generated/gui_guider.h
                generated/events_init.c
                generated/events_init.h
                generated/guider_customer_fonts/guider_customer_fonts.h
                generated/setup_scr_home_screen.c
                generated/setup_scr_incoming_call_screen.c
                generated/setup_scr_keypad_screen.c
                generated/setup_scr_settings_screen.c
                generated/setup_scr_call_screen.c
                generated/setup_scr_message_screen.c
                generated/widgets_init.c
                generated/widgets_init.h
                lvgl_examples/FreeRTOSConfig_frag.h
                lvgl_examples/lvgl_demo_utils.c
                lvgl_examples/lvgl_demo_utils.h
                lvgl_examples/lvgl_freertos.c
                lvgl_examples/lvgl_freertos.h
    )

    add_compile_definitions(LV_USE_IMG=1)
endif() # NOT DISABLE_LVGL_UI

mcux_add_source(
    SOURCES ipv4_ipv6_echo_common/shell_task.c
            ipv4_ipv6_echo_common/shell_task.h
            ipv4_ipv6_echo_common/shell_task_mode.h
            ipv4_ipv6_echo_common/socket_task.c
            ipv4_ipv6_echo_common/socket_task.h
)

mcux_add_source(
    SOURCES FreeRTOSConfig.h
            common/pin_mux.c
            common/pin_mux.h
            cred_flash_storage/cred_flash_storage.c
            cred_flash_storage/cred_flash_storage.h
            lwipopts.h
            lwippools.h
            main.c
            rdrw612bga/FreeRTOSConfigBoard.h
            wifi_config/wifi_config.h
            wlan_wrpr.c
            wlan_wrpr.h
            wifi_manager.c
            wifi_manager.c
)

mcux_add_source(
  BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
  SOURCES
    baresip/modules/account/account.c
    baresip/modules/auresamp/auresamp.c
    baresip/modules/ausine/ausine.c
    baresip/modules/echo/echo.c
    baresip/modules/g711/g711.c
    baresip/modules/nxp_audio/nxp_audio.c
    baresip/modules/opus/*.c
    baresip/modules/srtp/*.c
    baresip/src/*.c
)

mcux_add_source(
  BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
  SOURCES
    # rem
    libre/rem/aac/*.c
    libre/rem/au/*.c
    libre/rem/aubuf/*.c
    libre/rem/auconv/*.c
    libre/rem/aufile/*.c
    libre/rem/auframe/*.c
    libre/rem/aulevel/*.c
    libre/rem/aumix/*.c
    libre/rem/auresamp/*.c
    libre/rem/autone/*.c
    libre/rem/avc/*.c
    libre/rem/dtmf/*.c
    libre/rem/fir/*.c
    libre/rem/g711/*.c
    libre/rem/goertzel/*.c
    libre/rem/vid/*.c
    libre/rem/vidconv/*.c
    libre/rem/vidmix/*.c

    # src
    libre/src/aes/mbedtls3/*.c
    libre/src/async/*.c
    libre/src/av1/*.c
    libre/src/base64/*.c
    libre/src/bfcp/*.c
    libre/src/btrace/*.c
    libre/src/conf/*.c
    libre/src/crc32/*.c
    libre/src/dbg/*.c
    libre/src/dd/*.c
    libre/src/dns/*.c
    libre/src/fmt/*.c
    libre/src/h264/*.c
    libre/src/h265/*.c
    libre/src/hash/*.c
    libre/src/hmac/*.c
    libre/src/http/*.c
    libre/src/httpauth/*.c
    libre/src/ice/*.c
    libre/src/json/*.c
    libre/src/list/*.c
    libre/src/main/*.c
    libre/src/mbuf/*.c
    libre/src/md5/*.c
    libre/src/mem/*.c
    libre/src/mod/*.c
    libre/src/mqueue/*.c
    libre/src/mqueue/freertos/*.c
    libre/src/msg/*.c
    libre/src/net/*.c
    libre/src/net/freertos/*.c
    libre/src/odict/*.c
    libre/src/pcp/*.c
    libre/src/rtmp/*.c
    libre/src/rtp/*.c
    libre/src/rtpext/*.c
    libre/src/sa/*.c
    libre/src/sdp/*.c
    libre/src/sha/*.c
    libre/src/shim/*.c
    libre/src/sip/*.c
    libre/src/sipevent/*.c
    libre/src/sipreg/*.c
    libre/src/sipsess/*.c
    libre/src/srtp/*.c
    libre/src/stun/*.c
    libre/src/sys/*.c
    libre/src/tcp/*.c
    libre/src/telev/*.c
    libre/src/thread/*.c
    libre/src/tls/mbedtls3/*.c
    libre/src/tmr/*.c
    libre/src/trace/*.c
    libre/src/trice/*.c
    libre/src/turn/*.c
    libre/src/udp/*.c
    libre/src/unixsock/*.c
    libre/src/uri/*.c
    libre/src/websock/*.c
    certificates/server_crt_der.h
    certificates/server_key_der.h
)

# Select CA certificate header
if(USE_CUSTOM_CA)
    mcux_add_source(
        BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
        SOURCES certificates/my_ca.h
    )
    mcux_add_macro(CC "-DCA_CRT_DER_H=\\\"my_ca.h\\\"")
    message(STATUS "Using Custom CA certificate: certificates/my_ca.h")
else()
    mcux_add_source(
        BASE_PATH ${CMAKE_CURRENT_LIST_DIR}
        SOURCES certificates/ca_crt_der.h
    )
    mcux_add_macro(CC "-DCA_CRT_DER_H=\\\"ca_crt_der.h\\\"")
    message(STATUS "Using default CA certificate: certificates/ca_crt_der.h")
endif()

if(ENABLE_PRINT_CERTIFICATE_INFO)
    mcux_add_macro(CC "-DPRINT_CERTIFICATE_INFO\
                       -DMBEDTLS_ALLOW_PRIVATE_ACCESS"
                   )
    message(STATUS "TLS certificate printing enabled.")
endif()


mcux_add_include(
    INCLUDES .
             audio
             common
             baresip/include
             baresip/src
             libre/include
             libre/src/net/freertos
             libre/src/tls/mbedtls3
             nxp
             wifi_config
             certificates
)
mcux_add_macro(
    CC "-DUSE_RTOS=1\
        -DPRINTF_ADVANCED_ENABLE=1\
       -DLWIP_NETIF_API=1\
       -DHAVE_SELECT\
       -DLWIP_TIMEVAL_PRIVATE=0\
       -DPRINTF_ADVANCED_ENABLE=1\
       -DSTATIC\
       -DUSE_MBEDTLS\
       -DVER_MAJOR=0\
       -DVER_MINOR=1\
       -DVER_PATCH=0\
       -DUSE_TLS\
       -DUSE_RAM_BUFFER_CERTIFICATES\
       -DSERVER_MODEx\
       -DMTLS_VERIFY_CLIENTx\
       -DSHELL_BUFFER_SIZE=160U\
       -DSHELL_TASK_STACK_SIZE=(1024*8)\
       -DSTREAMER_ENABLE_AUDIO_SRC\
       -DSTREAMER_ENABLE_AUDIO_SINK"
)

#-------- add mbedtls configs -----------
mcux_remove_macro("-DMBEDTLS_CONFIG_FILE=\\\"mcux_mbedtls_psa_crypto_config.h\\\"")
mcux_add_macro(
    "-DMBEDTLS_CONFIG_FILE=\\\"mcux_mbedtls_custom_config.h\\\""
)
mcux_component_version(${mbedtls3x_version})
mcux_add_configuration(
	CC  "-DMBEDTLS_CONFIG_FILE=\\\"mcux_mbedtls_custom_config.h\\\""
)
mcux_add_source(
	SOURCES mcux_mbedtls_custom_config.h
	BASE_PATH ./libre/src/tls/mbedtls3
)
mcux_add_source(
	SOURCES port/mcux_mbedtls_accelerator_config.h
			port/mcux_psa_defines.h
	BASE_PATH ${SdkRootDirPath}/middleware/mbedtls3x/
)
mcux_add_include(
	INCLUDES port
	BASE_PATH ${SdkRootDirPath}/middleware/mbedtls3x/
)

mcux_add_iar_configuration(
	CC "--dlib_config full\
		--no_inline"
	CX "--no_clustering"
	LD "--semihosting"
)

mcux_add_iar_configuration(
    LD "--config_def=__stack_size__=0x400\
        --config_def=__heap_size__=0x400"
)

mcux_add_mdk_configuration(
    LD "--predefine=\"-D__stack_size__=0x400\"\
        --predefine=\"-D__heap_size__=0x400\""
)

mcux_add_armgcc_configuration(
    LD "-Xlinker --defsym=__stack_size__=2048\
        -Xlinker --defsym=__heap_size__=0x400"
)

mcux_remove_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS debug release flexspi_nor_debug flexspi_nor_release flash_debug flash_release
    LINKER ${board_root}/${board}/wifi_examples/common/linker/RW610_flash.ld
)

mcux_add_armgcc_linker_script(
    BASE_PATH .
    TARGETS debug flash_debug flash_release
    LINKER RW610_flash.ld
)

mcux_add_source(
    SOURCES common/app.h
            common/hardware_init.c
)

mcux_add_include(
    INCLUDES common
)
mcux_add_iar_configuration(
    CX "--no_clustering"
    TARGETS flash_release
    CC "--no_inline"
)

mcux_remove_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS debug release flexspi_nor_debug flexspi_nor_release flash_debug flash_release
    LINKER ${board_root}/${board}/wifi_examples/common/linker/RW612_wifi.ld
)

mcux_remove_iar_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS debug release flexspi_nor_debug flexspi_nor_release flash_debug flash_release
    LINKER ${board_root}/${board}/wifi_examples/common/linker/RW612_wifi.icf
)

mcux_remove_mdk_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS debug release flexspi_nor_debug flexspi_nor_release flash_debug flash_release
    LINKER ${board_root}/${board}/wifi_examples/common/linker/RW612_wifi.scf
)

mcux_add_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flash_debug flash_release
    LINKER ${board_root}/${board}/wifi_examples/common/linker/RW610_flash.ld
)

mcux_add_iar_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flash_debug flash_release
    LINKER ${board_root}/${board}/wifi_examples/common/linker/RW610_flash.icf
)

mcux_add_mdk_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flash_debug flash_release
    LINKER ${board_root}/${board}/wifi_examples/common/linker/RW610_flash.scf
)

mcux_remove_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flash_debug flash_release
    LINKER ${device_root}/${soc_portfolio}/${soc_series}/${device}/gcc/${CONFIG_MCUX_TOOLCHAIN_LINKER_DEVICE_PREFIX}_flash.ld
)

mcux_remove_iar_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flash_debug flash_release
    LINKER ${device_root}/${soc_portfolio}/${soc_series}/${device}/iar/${CONFIG_MCUX_TOOLCHAIN_LINKER_DEVICE_PREFIX}_flash.icf
)

mcux_remove_mdk_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flash_debug flash_release
    LINKER ${device_root}/${soc_portfolio}/${soc_series}/${device}/arm/${CONFIG_MCUX_TOOLCHAIN_LINKER_DEVICE_PREFIX}_flash.scf
)



mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)

mcux_add_include(INCLUDES rdrw612bga)

mcux_add_source(
    SOURCES cred_flash_storage/cred_flash_storage.c
            cred_flash_storage/cred_flash_storage.h
)

mcux_add_include(
    INCLUDES cred_flash_storage
)

mcux_add_source(
    SOURCES audio/app_streamer.c
            audio/app_streamer.h
            audio/app_definitions.h
            audio/streamer_pcm.c
            audio/streamer_pcm.h
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES
        middleware/audio_voice/maestro/src/elements/audio_src.c
        middleware/audio_voice/maestro/src/elements/audio_sink.c
        middleware/audio_voice/maestro/src/devices/audio_src_default.c
        middleware/audio_voice/maestro/src/devices/audio_sink_default.c
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES
        middleware/audio_voice/maestro/src/elements
        middleware/audio_voice/maestro/src/devices
)
