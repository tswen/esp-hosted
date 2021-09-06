set(srcs)
set(icls)

# common
list(APPEND srcs
    "../../../common/esp_hosted_config.pb-c.c"
    )

# host common
list(APPEND srcs
    "../../host_common/commands.c"
    "../../host_common/transport_pserial.c"
    )

# other
list(APPEND srcs
    "dongle_main.c"
    "CLI_Commands.c"
    "platform_wrapper.c"
    )

list(APPEND srcs
    "host_control/control.c"
    )

list(APPEND srcs
    "other/Command_Parse.c"
    "other/data_back.c"
    "other/uart.c"
    )

list(APPEND srcs
    "common/common.c"
    "common/util.c"
    )

list(APPEND srcs
    "driver/net/netdev_api.c"
    "driver/net/netdev_stub.c"
    "driver/serial/serial_if.c"
    "driver/sdspi_driver/sdspi_drv.c"
    )

list(APPEND srcs
    "host_wifi/src/host_wifi_default.c"
    "host_wifi/src/host_wifi_netif.c"
    "host_wifi/src/esp_host_pkt.c"
    )

list(APPEND icls
    "../../../common/include"
    "../../host_common/include"
    "host_control/include"
    "other/include"
    "common/include"
    "driver/include"
    "host_wifi/include"
    )

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${icls}
                       )
