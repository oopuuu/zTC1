#include "mico.h"
#include "SocketUtils.h"
#include "json_c/json.h"
#include "mqtt_server/user_mqtt_client.h"
#include "user_power.h"
#include "main.h"
#include "user_gpio.h"
#include "user_wifi.h"
#include "http_server/app_httpd.h"


#define UDP_SERVER_IP       "192.168.31.226"
#define UDP_SERVER_PORT     2738
#define DEVICE_ID           "plug01"
#define AUTH_KEY            "test"

static int udp_fd = -1;
static struct sockaddr_in server_addr;
static bool is_authenticated = false;

void send_udp_json(json_object *json) {
    if (udp_fd < 0) return;

    const char *msg = json_object_to_json_string(json);
    sendto(udp_fd, msg, strlen(msg), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
}

void send_login_packet(void) {
    json_object *j = json_object_new_object();
    json_object_object_add(j, "type", json_object_new_string("login"));
    json_object_object_add(j, "auth_key", json_object_new_string(AUTH_KEY));
    json_object_object_add(j, "device_id", json_object_new_string(DEVICE_ID));

    send_udp_json(j);
    json_object_put(j);
}

void send_status_packet(const char *status) {
    if (!is_authenticated) return;

    json_object *j = json_object_new_object();
    json_object_object_add(j, "type", json_object_new_string("status"));
    json_object_object_add(j, "auth_key", json_object_new_string(AUTH_KEY));
    json_object_object_add(j, "device_id", json_object_new_string(DEVICE_ID));
    json_object_object_add(j, "status", json_object_new_string(status));

    send_udp_json(j);
    json_object_put(j);
}

void handle_udp_msg(char *msg) {
    json_object *root = json_tokener_parse(msg);
    if (!root) return;

    const char *type = json_object_get_string(json_object_object_get(root, "type"));

    if (strcmp(type, "login_ack") == 0) {
        const char *result = json_object_get_string(json_object_object_get(root, "result"));
        if (strcmp(result, "ok") == 0) {
            is_authenticated = true;
        }
    } else if (strcmp(type, "control") == 0 && is_authenticated) {
        const char *cmd = json_object_get_string(json_object_object_get(root, "command"));
        // TODO: 控制插座硬件
        if (strcmp(cmd, "turn_on") == 0) {
            // gpio_output_high(PLUG_PIN);
        } else if (strcmp(cmd, "turn_off") == 0) {
            // gpio_output_low(PLUG_PIN);
        }
    }

    json_object_put(root);
}

void udp_recv_loop(void) {
    char buf[256];
    while (1) {
        if (udp_fd >= 0) {
            int len = recvfrom(udp_fd, buf, sizeof(buf) - 1, 0, NULL, 0);
            if (len > 0) {
                buf[len] = '\0';
                handle_udp_msg(buf);
            }
        }
        mico_thread_sleep(1);
    }
}

void udp_heartbeat_loop(void) {
    while (1) {
        if (udp_fd < 0) {
            udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (udp_fd < 0) {
                mico_thread_sleep(3);
                continue;
            }

            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(UDP_SERVER_PORT);
            server_addr.sin_addr.s_addr = inet_addr(UDP_SERVER_IP);
        }

        if (!is_authenticated) {
            send_login_packet();
        } else {
            char *sockets = GetSocketStatus();
            char *short_click_config = GetButtonClickConfig();
            char *tc1_status = malloc(1500);
            char *socket_names = malloc(512);
            sprintf(socket_names, "%s,%s,%s,%s,%s,%s",
                    user_config->socket_names[0],
                    user_config->socket_names[1],
                    user_config->socket_names[2],
                    user_config->socket_names[3],
                    user_config->socket_names[4],
                    user_config->socket_names[5]);
            sprintf(tc1_status, TC1_STATUS_JSON, sockets, ip_status.mode,
                    sys_config->micoSystemConfig.ssid, sys_config->micoSystemConfig.user_key,
                    user_config->ap_name, user_config->ap_key, MQTT_SERVER, MQTT_SERVER_PORT,
                    MQTT_SERVER_USR, MQTT_SERVER_PWD,
                    VERSION, ip_status.ip, ip_status.mask, ip_status.gateway, user_config->mqtt_report_freq,
                    user_config->power_led_enabled, 0L, socket_names, childLockEnabled,
                    sys_config->micoSystemConfig.name, short_click_config);
            send_status_packet(tc1_status);
            if (socket_names) free(socket_names);
            if (tc1_status) free(tc1_status);
        }

        mico_thread_sleep(5);
    }
}

extern void udp_server_start(void) {
    mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "udp_recv", udp_recv_loop, 0x800, NULL);
    mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "udp_send", udp_heartbeat_loop, 0x800, NULL);
}