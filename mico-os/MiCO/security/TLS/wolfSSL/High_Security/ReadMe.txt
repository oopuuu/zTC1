用于SSL认证的库，对ssl做了一些设置修改：
1、支持CRL
2、会对证书的有效期进行验证，系统时间通过调用mico_time_get_utc_time_ms来获取系统的时间，用来验证证书有效期。
3、最低的RSA bits定义为2048
4、证书验证时关闭SHA，但是cipher list需要支持SHA
5、TLS 1.2，NO_OLD_TLS

要SSL使用这个库，需要在make命令里面增加：HIGH_SECURITY=1
比如：
mico make wifi.station@MK3060 HIGH_SECURITY=1