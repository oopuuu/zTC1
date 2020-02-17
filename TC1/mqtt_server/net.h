
void UserSend(int udp_flag, char *s)
{
    if (udp_flag || !UserMqttIsConnect())
        UserUdpSend(s); //发送数据
    else
        UserMqttSend(s);
}
