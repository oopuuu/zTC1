#include "http_server/web_log.h"
#define os_log(format, ...) do { custom_log("FUNCTION", format, ##__VA_ARGS__); web_log(format, ##__VA_ARGS__) } while(0)

#include "TimeUtils.h"

#include "main.h"
#include "user_gpio.h"
#include "c_json/c_json.h"
#include "ota_server/user_ota.h"
#include "user_mqtt_client.h"
#include "user_udp.h"

uint32_t last_time = 0;
bool JsonSocketAnalysis(int udp_flag, unsigned char x, cJSON * pJsonRoot, cJSON * pJsonSend);

void UserFunctionSetLastTime()
{
    last_time = UpTicks();
}

void UserSend(int udp_flag, char *s)
{
    if (udp_flag || !UserMqttIsConnect())
        UserUdpSend(s); //发送数据
    else
        UserMqttSend(s);
}

void UserFunctionCmdReceived(int udp_flag, char* pusrdata)
{

    unsigned char i;
    bool update_user_config_flag = false;   //标志位,记录最后是否需要更新储存的数据
    bool return_flag = true;    //为true时返回json结果,否则不返回

    cJSON * pJsonRoot = cJSON_Parse(pusrdata);
    if (!pJsonRoot)
    {
        os_log("this is not a json data:\r\n%s\r\n", pusrdata);
        return;
    }

    //解析UDP命令device report(MQTT同样回复命令)
    cJSON *p_cmd = cJSON_GetObjectItem(pJsonRoot, "cmd");
    if (p_cmd && cJSON_IsString(p_cmd) && strcmp(p_cmd->valuestring, "device report") == 0)
    {
        cJSON *pRoot = cJSON_CreateObject();
        cJSON_AddStringToObject(pRoot, "name", sys_config->micoSystemConfig.name);
        cJSON_AddStringToObject(pRoot, "mac", str_mac);
        cJSON_AddNumberToObject(pRoot, "type", TYPE);
        cJSON_AddStringToObject(pRoot, "type_name", TYPE_NAME);

        IPStatusTypedef para;
        micoWlanGetIPStatus(&para, Station);
        cJSON_AddStringToObject(pRoot, "ip", para.ip);

        char *s = cJSON_Print(pRoot);
        UserSend(udp_flag, s); //发送数据
        free((void *) s);
        cJSON_Delete(pRoot);
    }

    //以下为解析命令部分
    cJSON *p_name = cJSON_GetObjectItem(pJsonRoot, "name");
    cJSON *p_mac = cJSON_GetObjectItem(pJsonRoot, "mac");

    //开始正式处理所有命令
    if ((p_name && cJSON_IsString(p_name) && strcmp(p_name->valuestring, sys_config->micoSystemConfig.name) == 0)    //name
         || (p_mac && cJSON_IsString(p_mac) && strcmp(p_mac->valuestring, str_mac) == 0)   //mac
       )
    {
        cJSON *json_send = cJSON_CreateObject();
        cJSON_AddStringToObject(json_send, "mac", str_mac);

        //解析重启命令
        if(p_cmd && cJSON_IsString(p_cmd) && strcmp(p_cmd->valuestring, "restart") == 0)
        {
            os_log("cmd:restart");
            mico_system_power_perform(mico_system_context_get(), eState_Software_Reset);
        }

        //解析版本
        cJSON *p_version = cJSON_GetObjectItem(pJsonRoot, "version");
        if (p_version)
        {
            os_log("version:%s",VERSION);
            cJSON_AddStringToObject(json_send, "version", VERSION);
        }
        //解析运行时间
        cJSON *p_total_time = cJSON_GetObjectItem(pJsonRoot, "total_time");
        if (p_total_time)
        {
            cJSON_AddNumberToObject(json_send, "total_time", total_time);
        }
        //解析功率
        cJSON *p_power = cJSON_GetObjectItem(pJsonRoot, "power");
        if (p_power)
        {
            char *temp_buf = malloc(16);
            if (temp_buf != NULL)
            {
                sprintf(temp_buf, "%d.%d", (int)(power/10), (int)(power%10));
                cJSON_AddStringToObject(json_send, "power", temp_buf);
                free(temp_buf);
            }
            os_log("power:%d", (int)power);
        }
        //解析主机setting-----------------------------------------------------------------
        cJSON *p_setting = cJSON_GetObjectItem(pJsonRoot, "setting");
        if (p_setting)
        {
            //解析ota
            cJSON *p_ota = cJSON_GetObjectItem(p_setting, "ota");
            if (p_ota)
            {
                if (cJSON_IsString(p_ota))
                    UserOtaStart(p_ota->valuestring, NULL);
            }

            cJSON *json_setting_send = cJSON_CreateObject();
            //设置设备名称/deviceid
            cJSON *p_setting_name = cJSON_GetObjectItem(p_setting, "name");
            if (p_setting_name && cJSON_IsString(p_setting_name))
            {
                update_user_config_flag = true;
                sprintf(sys_config->micoSystemConfig.name, p_setting_name->valuestring);
            }

            //设置mqtt ip
            cJSON *p_mqtt_ip = cJSON_GetObjectItem(p_setting, "mqtt_uri");
            if (p_mqtt_ip && cJSON_IsString(p_mqtt_ip))
            {
                update_user_config_flag = true;
                sprintf(user_config->mqtt_ip, p_mqtt_ip->valuestring);
            }

            //设置mqtt port
            cJSON *p_mqtt_port = cJSON_GetObjectItem(p_setting, "mqtt_port");
            if (p_mqtt_port && cJSON_IsNumber(p_mqtt_port))
            {
                update_user_config_flag = true;
                user_config->mqtt_port = p_mqtt_port->valueint;
            }

            //设置mqtt user
            cJSON *p_mqtt_user = cJSON_GetObjectItem(p_setting, "mqtt_user");
            if (p_mqtt_user && cJSON_IsString(p_mqtt_user))
            {
                update_user_config_flag = true;
                sprintf(user_config->mqtt_user, p_mqtt_user->valuestring);
            }

            //设置mqtt password
            cJSON *p_mqtt_password = cJSON_GetObjectItem(p_setting, "mqtt_password");
            if (p_mqtt_password && cJSON_IsString(p_mqtt_password))
            {
                update_user_config_flag = true;
                sprintf(user_config->mqtt_password, p_mqtt_password->valuestring);
            }

            //开发返回数据
            //返回设备ota
            if (p_ota) cJSON_AddStringToObject(json_setting_send, "ota", p_ota->valuestring);

            //返回设备名称/deviceid
            if (p_setting_name) cJSON_AddStringToObject(json_setting_send, "name", sys_config->micoSystemConfig.name);
            //返回mqtt ip
            if (p_mqtt_ip) cJSON_AddStringToObject(json_setting_send, "mqtt_uri", user_config->mqtt_ip);
            //返回mqtt port
            if (p_mqtt_port) cJSON_AddNumberToObject(json_setting_send, "mqtt_port", user_config->mqtt_port);
            //返回mqtt user
            if (p_mqtt_user) cJSON_AddStringToObject(json_setting_send, "mqtt_user", user_config->mqtt_user);
            //返回mqtt password
            if (p_mqtt_password) cJSON_AddStringToObject(json_setting_send, "mqtt_password", user_config->mqtt_password);

            cJSON_AddItemToObject(json_send, "setting", json_setting_send);
        }

        //解析socket-----------------------------------------------------------------
        for (i = 0; i < SOCKET_NUM; i++)
        {
            if (JsonSocketAnalysis(udp_flag, i, pJsonRoot, json_send))
                update_user_config_flag = true;
        }

        cJSON_AddStringToObject(json_send, "name", sys_config->micoSystemConfig.name);

        if (return_flag == true)
        {
            char *json_str = cJSON_Print(json_send);
            UserSend(udp_flag, json_str); //发送数据
            free((void *) json_str);
        }
        cJSON_Delete(json_send);
    }

    if (update_user_config_flag)
    {
        mico_system_context_update(sys_config);
        update_user_config_flag = false;
    }

    cJSON_Delete(pJsonRoot);

}

/*
 *解析处理定时任务json
 *udp_flag:发送udp/mqtt标志位,此处修改插座开关状态时,需要实时更新给domoticz
 *x:插座编号
 */
bool JsonSocketAnalysis(int udp_flag, unsigned char x, cJSON * pJsonRoot, cJSON * pJsonSend)
{
    if (!pJsonRoot) return false;
    if (!pJsonSend) return false;
    bool return_flag = false;
    char socket_str[] = "socket_X";
    socket_str[5] = x + '0';

    cJSON *p_socket = cJSON_GetObjectItem(pJsonRoot, socket_str);
    if (!p_socket) return_flag = false;

    cJSON *json_socket_send = cJSON_CreateObject();

    //解析socket on------------------------------------------------------
    if (p_socket)
    {
        cJSON *p_socket_on = cJSON_GetObjectItem(p_socket, "on");
        if (p_socket_on)
        {
            if (cJSON_IsNumber(p_socket_on))
            {
                UserRelaySet(x, p_socket_on->valueint);
                return_flag = true;
            }
            UserMqttSendSocketState(x);
        }

        //解析socket中setting项目----------------------------------------------
        cJSON *p_socket_setting = cJSON_GetObjectItem(p_socket, "setting");
        if (p_socket_setting)
        {
            cJSON *json_socket_setting_send = cJSON_CreateObject();
            //解析socket中setting中name----------------------------------------
            cJSON *p_socket_setting_name = cJSON_GetObjectItem(p_socket_setting, "name");
            if (p_socket_setting_name)
            {
                if (cJSON_IsString(p_socket_setting_name))
                {
                    return_flag = true;
                    sprintf(user_config->socket_configs[x].name, p_socket_setting_name->valuestring);
                    UserMqttHassAutoName(x);
                }
                cJSON_AddStringToObject(json_socket_setting_send, "name", user_config->socket_configs[x].name);
            }

            cJSON_AddItemToObject(json_socket_send, "setting", json_socket_setting_send);
        }
    }
    cJSON_AddNumberToObject(json_socket_send, "on", user_config->socket_configs[x].on);

    cJSON_AddItemToObject(pJsonSend, socket_str, json_socket_send);
    return return_flag;
}

unsigned char StrToHex(char a, char b)
{
    if (a >= 0x30 && a <= 0x39)
    {
        a -= 0x30;
    }
    else if (a >= 0x41 && a <= 0x46)
    {
        a = a + 10 - 0x41;
    }
    else if (a >= 0x61 && a <= 0x66)
    {
        a = a + 10 - 0x61;
    }

    if (b >= 0x30 && b <= 0x39)
    {
        b -= 0x30;
    }
    else if (b >= 0x41 && b <= 0x46)
    {
        b = b + 10 - 0x41;
    }
    else if (b >= 0x61 && b <= 0x66)
    {
        b = b + 10 - 0x61;
    }

    return a * 16 + b;
}
