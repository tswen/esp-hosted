# Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from commands_lib import *
import argparse
#import pigpio
import time
from datetime import datetime
import RPi.GPIO as GPIO

failure = 'failure'
#success = "success"
HB_MAX_RETRY = 4
ESP_HB_INTERVAL_SEC = 60
ESP_HB_RETRY_INTERVAL_SEC = 5

exp_hb_num = 0
hb_num = 0
recvd_hb_num = 0
retry = 0;
usleep = lambda x: time.sleep(x/1000000.0)


# functions
def get_curr_time_str():
    date = datetime.now()
    time = date.time()
    now = date.strftime("%d-%m ") + time.strftime("%H:%M:%S : ")
    return now

def reset_esp32():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(args.resetpin , GPIO.OUT)
    GPIO.output(args.resetpin, GPIO.HIGH)
    usleep(100)
    GPIO.output(args.resetpin, GPIO.LOW)
    usleep(100)
    GPIO.output(args.resetpin, GPIO.HIGH)

# functions over


# start the script
parser = argparse.ArgumentParser(description='hearbeat.py script monitors ESP32 by sending periodic heartbeat request')

parser.add_argument("--resetpin", type=int, default=6, help="Pin used to reset ESP32 (default: 6)")

args = parser.parse_args()

print(get_curr_time_str() + "--- Starting heartbeats for ESP32 ---")

while True:
    control_path_platform_init()

    exp_hb_num = hb_num

    recvd_hb_num = esp_hb(hb_num)

    if (recvd_hb_num == failure):
        retry = retry + 1
        print(get_curr_time_str() + "retry left: " + str(HB_MAX_RETRY-retry))
        if(retry >= HB_MAX_RETRY):
            print(get_curr_time_str() + "--- HB timed-out, Reseting ESP32.. Setup Wi-Fi again ---")
            reset_esp32()
            time.sleep(5)
            hb_num = 0
            retry = 0
            #firmware loading time
            time.sleep(5)
        else:
            time.sleep(ESP_HB_RETRY_INTERVAL_SEC)
        continue;

    elif ((recvd_hb_num != exp_hb_num) and (recvd_hb_num == 0)):

        print(get_curr_time_str() + "---- ESP32 identified already rebooted. Seup Wi-Fi again ----")
        hb_num = recvd_hb_num+1

    else:

        hb_num = hb_num + 1
        if (hb_num%4==0):
                print(get_curr_time_str() + "Heartbeat OK")

    if (retry):
        print(get_curr_time_str() + "Heartbeat restored")
        retry = 0;

    # Sleep and again send HB
    time.sleep(ESP_HB_INTERVAL_SEC)
