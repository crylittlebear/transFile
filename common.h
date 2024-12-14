#ifndef __COMMON_H__
#define __COMMON_H__

#include <QString>
#include <QNetworkAccessManager>

#include "des.h"

// 用户名、密码、电话号码、邮箱、IP、端口的正则表达式
#define USER_REG	"^[a-zA-Z\\d_@#-\*]\{3,16}$"
#define PASSWD_REG	"^[a-zA-Z\\d_@#-\*]\{6,18}$"
#define PHONE_REG	"1\\d{10}"
#define EMAIL_REG	"^[a-zA-Z\\d\._-]\+@[a-zA-Z\\d_\.-]\+(\.[a-zA-Z0-9_-]\+)+$"
#define IP_REG		"((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)"
#define PORT_REG	"^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9])|(^[1-6][0-5][0-5][0-3][0-5]$)"

// 配置文件路径
#define CONF_FILE       "conf/cfg.json"
#define FILE_TYPE_DIR   "conf/fileType"
#define RECORD_DIR      "conf/record"

enum TransformStatus {
    StatusUpload,
    StatusDownload,
    StatusRecord
};

class Common
{
public:
	// 获取单例
	static Common* instance();

	// 写入登录数据到文件
    void writeLoginInfo(QString user, QString pwd, bool isRemember,
                        QString path = CONF_FILE);

	// 服务器数据到文件
    void writeWebInfo(QString ip, QString port, QString path = CONF_FILE);

	// 从文件中读取数据
    QString getConfigValue(QString title, QString key, QString path = CONF_FILE);

	// 加密字符串
	QString getStrMD5(QString str);

    // 获取某个文件的md5码
    QString getFileMd5(QString filePath);

    // 获取分隔符
    QString getBoundary();

    // 获取网络接口管理类
    QNetworkAccessManager* getNetworkAccessManager();

    // 获取网盘支持的文件类型
    void getFileTypeList();

    // 从文件名获取文件的类型
    QString getFileType(QString fileTypeName);

    //传输数据记录到本地文件
    //user:操作用户
    //fileName:操作的文件
    //code:操作码
    //path:文件保存的路径
    void writeRecord(QString user, QString fileName, QString code, QString path = RECORD_DIR);

    //返回操作名称
    QString getActionStrring(QString code);

    //等待
    void sleep(unsigned int msec);

private:
	Common();

	static Common* instance_;

    QNetworkAccessManager *m_manager;

    QStringList m_fileTypeList;
};

#endif // __COMMON_H__
