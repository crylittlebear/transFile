#include "logininfoinstance.h"

LoginInfoInstance* LoginInfoInstance::instance_ = new LoginInfoInstance;

LoginInfoInstance *LoginInfoInstance::instance()
{
    return instance_;
}

void LoginInfoInstance::setUser(QString user)
{
    user_ = user;
}

void LoginInfoInstance::setToken(QString token)
{
    token_ = token;
}

void LoginInfoInstance::setIp(QString ip)
{
    ip_ = ip;
}

void LoginInfoInstance::setPort(QString port)
{
    port_ = port;
}

QString LoginInfoInstance::user() const
{
    return user_;
}

QString LoginInfoInstance::token() const
{
    return token_;
}

QString LoginInfoInstance::ip() const
{
    return ip_;
}

QString LoginInfoInstance::port() const
{
    return port_;
}
