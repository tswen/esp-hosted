# Python Demo Application

[test.py](../host/linux/host_control/python_support/test.py) is a demo application to test control path interface:

It uses APIs present in [test_api.py](../host/linux/host_control/python_support/test_api.py). User should first modify configuration parameters in [test_config.py](../host/linux/host_control/python_support/test_config.py).

Note:-
Please execute `test.py` as below.

```
sudo python test.py
```
Note:
* After `test_station_mode_connect` API call, User needs to run DHCP client to obtain IP address from an external AP. Then network data path will be open for higher applications to use `ethsta0` interface for data communication. For an example as below.

```
sudo dhclient ethsta0 -r

sudo dhclient ethsta0 -v
```

* After `test_softap_mode_start` API ,to start data connection, set up a DHCP server on the Raspberry Pi, or configure a static IP address for AP interface (`ethap0`). For an example as below:

```
sudo dnsmasq --no-daemon --no-resolv --no-poll --dhcp-script=/system/bin/dhcp_announce --dhcp-range=192.168.4.1,192.168.4.20,1h

sudo ifconfig ethap0 192.168.4.5
```

# Python stress Application

[stress.py](../host/linux/host_control/python_support/stress.py) use for stress testing of control path APIs. User should first modify configuration parameters in [test_config.py](../host/linux/host_control/python_support/test_config.py). `STRESS_TEST_COUNT` variable is defined in `stress.py` for number of iterations for stress testing.

Note:-
Please execute `stress.py` as below.
```
ex.
sudo python stress.py
```

# Heartbeat Application

[heartbeat.py](../host/linux/host_control/python_support/heartbeat.py) used for monitoring liveliness of ESP32. It sends a sequential number as heartbeat and expects a heartbeat reponse within `TIMEOUT_PSERIAL_RESP_HB` time. ESP32 maintains own sequential number in reference to the heartbeat requests received. In case the ESP32 is rebooted, it starts sequence number of `0`. This is used to understand if ESP32 is silently rebooted. Heartbeat is sent in loop after every `ESP_HB_INTERVAL_SEC` interval. In case heartbeat is missed, this interval is reduced to `ESP_HB_RETRY_INTERVAL_SEC` and retried for `HB_MAX_RETRY` times. In case the heartbeats response not received after all retries, this application assumes that ESP32 is stuck in processing and resets ESP32 using `EN` or `RST` physical pin

## Parameters
- `resetpin`:
This is optional parameter and defaults to `6` in Raspberry Pi case. This pin number when passed, the application will use that to reset ESP32.

To execute this process,
```
python3 ./heartbeat.py --resetpin=6
```
or

```
python3 ./heartbeat.py
```
