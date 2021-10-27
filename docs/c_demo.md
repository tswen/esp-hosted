# C Demo Application

[test.c](../host/linux/host_control/c_support/test.c) is a demo application to provide basic command line arguments as follows:

| Command line argument | Operation |
|:----|:----|
| sta_connect | Connect ESP32 station to external AP, assign MAC address of ESP32 station to `ethsta0` and up `ethsta0` interface |
| sta_disconnect | Disconnect ESP32 station from external AP and down `ethsta0` interface |
| ap_start | Start ESP32 softAP, assign MAC address of ESP32 softAP to `ethap0` and up `ethap0` interface |
| ap_stop | Stop ESP32 softAP stop and down `ethap0` interface |
| scan | Scan external access points |
| sta_list | List external stations connected to softAP |

It uses APIs present in [test_api.c](../host/linux/host_control/c_support/test_api.c). User should first modify configuration parameters in [test_config.h](../host/linux/host_control/c_support/test_config.h). Then run `make` in [c_support](../host/linux/host_control/c_support) to compile `test.c`.

Note:-
Please execute `test.out` as below.

```
ex.
sudo ./test.out sta_connect sta_disconnect ap_start ap_stop scan sta_list
```
Note:
* After `sta_connect`, User needs to run DHCP client to obtain IP address from an external AP. Then network data path will be open for higher applications to use `ethsta0` interface for data communication. For an example as below.

```
sudo dhclient ethsta0 -r

sudo dhclient ethsta0 -v
```

* After `ap_start` to start data connection, set up a DHCP server on the Raspberry Pi, or configure a static IP address for AP interface (`ethap0`). For an example as below:

```
sudo dnsmasq --no-daemon --no-resolv --no-poll --dhcp-script=/system/bin/dhcp_announce --dhcp-range=192.168.4.1,192.168.4.20,1h

sudo ifconfig ethap0 192.168.4.5
```

# C stress Application

[stress.c](../host/linux/host_control/c_support/stress.c) use for stress testing of control path APIs. It provides basic command line arguments as follows:

| Command line argument | Operation |
|:----|:----|
| Any positive interger number | Number of iterations for stress test |
| sta_connect | Connect ESP32 station to external AP, assign MAC address of ESP32 station to `ethsta0` and up `ethsta0` interface |
| sta_disconnect | Disconnect ESP32 station from external AP and down `ethsta0` interface |
| ap_start | Start ESP32 softAP, assign MAC address of ESP32 softAP to `ethap0` and up `ethap0` interface |
| ap_stop | Stop ESP32 softAP stop and down `ethap0` interface |
| scan | Scan external access points |
| sta_list | List external stations connected to softAP |

Run `make stress` in [c_support](../host/linux/host_control/c_support) directory to compile `stress.c`.

Note:-
Please execute `stress.out` as below.

```
ex.
sudo ./stress.out 1 sta_connect sta_disconnect ap_start ap_stop scan sta_list
```

# Heartbeat Application

[heartbeat.c](../host/linux/host_control/c_support/heartbeat.c) used for monitoring liveliness of ESP32. It sends a sequential number as heartbeat and expects a heartbeat reponse within `TIMEOUT_PSERIAL_RESP_HB` time. ESP32 maintains own sequential number in reference to the heartbeat requests received. In case the ESP32 is rebooted, it starts sequence number of `0`. This is used to understand if ESP32 is silently rebooted. Heartbeat is sent in loop after every `ESP_HB_INTERVAL_SEC` interval. In case heartbeat is missed, this interval is reduced to `ESP_HB_RETRY_INTERVAL_SEC` and retried for `HB_MAX_RETRY` times. In case the heartbeats response not received after all retries, this application assumes that ESP32 is stuck in processing and resets ESP32 using `EN` or `RST` physical pin

Run `make heartbeat` in [c_support](../host/linux/host_control/c_support) directory to compile `heartbeat.c`.

## Parameters
- `esp_reset_gpio_pin_num`:
This is optional parameter and defaults to `6` in Raspberry Pi case. This pin number when passed, the application will use that to reset ESP32.

To execute this process,
```
sudo ./heartbeat.out 6
```
or

```
sudo ./heartbeat.out
```
