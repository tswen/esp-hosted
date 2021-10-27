/*
 * Espressif Systems Wireless LAN device driver
 *
 * Copyright (C) 2015-2021 Espressif Systems (Shanghai) PTE LTD
 *
 * This software file (the "File") is distributed by Espressif Systems (Shanghai)
 * PTE LTD under the terms of the GNU General Public License Version 2, June 1991
 * (the "License").  You may use, redistribute and/or modify this File in
 * accordance with the terms and conditions of the License, a copy of which
 * is available by writing to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
 * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
 * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
 * this warranty disclaimer.
 */

#include <unistd.h>
#include "test_api.h"
#include <limits.h>
#include <pigpio.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

#define ESP_HB_INTERVAL_SEC       60
#define ESP_HB_RETRY_INTERVAL_SEC 5
#define DATE_STR_SIZE             16
#define HB_MAX_RETRY              4

/* Raspberry specific, BCM6 -> physical pin 31 */
static int resetpin = 6;

/* heartbeat specific data */
static uint32_t hb_num;
static uint32_t exp_hb_num;
static uint32_t recvd_hb_num;

static int gpio_fd = 0;

static int gpio_init(void)
{
    /* 1. Process pigpiod should be killed (if running) using
     * sudo killall pigpiod
     * as this process take control of gpio
     * In that case, pigpiod instance here fails
     *
     * 2. gpiod needs sudo access. without which it cannot be run
     * This is the reason why current script needs sudo access
     */
   if (gpioInitialise() < 0)
   {
      fprintf(stderr, "pigpio initialisation failed\n");
      exit(-1);
   }

   gpioSetMode(resetpin, PI_OUTPUT);

   return 0;
}

static int reset_esp32(void)
{
    gpioWrite(resetpin, 1);
    usleep(100);
    gpioWrite(resetpin, 0);
    usleep(100);
    gpioWrite(resetpin, 1);

    return 0;
}

static char * get_curr_time_str(char*str)
{
    time_t now;
    struct tm tm;

    now = time(NULL);
    tm = *localtime(&now);
    if (str) {
      memset(str, '\0', DATE_STR_SIZE);
      snprintf(str, DATE_STR_SIZE, "%02d-%02d %02d:%02d:%02d",
          tm.tm_mday, tm.tm_mon + 1, tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    return str;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    char * strtol_ptr = NULL;
    char date[DATE_STR_SIZE] = {'\0'};
    uint8_t retry = 0;

    if (argc > 1) {

      if ((0 == strncasecmp("--help", argv[1], sizeof("--help"))) ||
          (0 == strncasecmp("-h", argv[1], sizeof("-h")))) {
        printf("Usage: %s [<esp_reset_gpio_pin_num>]\n", argv[0]);
        printf("           esp_reset_gpio_pin_num default value: 6\n");
        return 0;
      }

      resetpin = strtol(argv[1], &strtol_ptr, 10);
      if (errno == EINVAL || resetpin == LONG_MIN || resetpin == LONG_MAX) {
        printf("Usage: %s [<esp_reset_gpio_pin_num>]\n", argv[0]);
        printf("           esp_reset_gpio_pin_num default value: 6\n");
        return 0;
      }

      printf("Resetpin to be used: %u\n",resetpin);
    }

    gpio_init();

    hb_num = 0;
    printf("%s: --- Starting heartbeats for ESP32 ---\n",get_curr_time_str(date));

    while (1) {

        ret = control_path_platform_init();
        if (ret != SUCCESS) {
          printf("Failed to initialize serial interface, exiting!!!!\n");
          close(gpio_fd);
          return -1;
        }

        exp_hb_num = hb_num;

        ret = esp_hb(hb_num, &recvd_hb_num);
        if (ret != SUCCESS) {

            retry++;
            printf("%s: retry left: %u\n", get_curr_time_str(date), HB_MAX_RETRY-retry);
            if(retry >= HB_MAX_RETRY) {
                printf("%s: --- HB timed-out, Reseting ESP32.. Setup Wi-Fi again ---\n",
                   get_curr_time_str(date));
                reset_esp32();
                sleep(5);
                hb_num = 0;
                retry = 0;
            } else {
                sleep(ESP_HB_RETRY_INTERVAL_SEC);
            }
            continue;

        } else if ((recvd_hb_num != exp_hb_num) && (recvd_hb_num == 0)) {

            printf("%s: ---- ESP32 identified already rebooted. Seup Wi-Fi again ----\n",
                get_curr_time_str(date));
            hb_num = recvd_hb_num+1;

        } else {

            /* printf("recvd hb %u exp hb %u\n", recvd_hb_num, exp_hb_num); */
            hb_num++;
            if (hb_num%4==0) {
                printf("%s Heartbeat OK\n",get_curr_time_str(date));
            }
        }
        if (retry) {
            printf("%s Heartbeat restored\n",get_curr_time_str(date));
            retry = 0;
        }

        /* Sleep and again send HB */
        sleep(ESP_HB_INTERVAL_SEC);
    }

    return 0;
}
