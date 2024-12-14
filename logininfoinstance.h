#ifndef LOGININFOINSTANCE_H
#define LOGININFOINSTANCE_H

#include <QObject>

class LoginInfoInstance : public QObject
{
    Q_OBJECT
public:
    static LoginInfoInstance* instance();

    void setUser(QString user);
    void setToken(QString token);
    void setIp(QString ip);
    void setPort(QString port);

    QString user() const;
    QString token() const;
    QString ip() const;
    QString port() const;

private:
    LoginInfoInstance() = default;

private:
    QString user_;
    QString token_;
    QString ip_;
    QString port_;

    static LoginInfoInstance* instance_;
};

#endif // LOGININFOINSTANCE_H
