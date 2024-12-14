#include "common.h"

#include "qcryptographichash.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonvalue.h"
#include "qmap.h"
#include "qfile.h"
#include <QTime>
#include <QCoreApplication>
#include <QRandomGenerator>
#include <QDir>

#include "logger.h"

Common* Common::instance_ = new Common;

Common::Common()
{
    m_manager = new QNetworkAccessManager();
    getFileTypeList();
}

Common* Common::instance()
{
	return instance_;
}

/**
    将登录的信息写入到配置文件cfg.jgon
*/
void Common::writeLoginInfo(QString user, QString pwd, bool isRemember, QString path)
{
    QString ip = getConfigValue("web_server", "ip");
    QString port = getConfigValue("web_server", "port");
	QMap<QString, QVariant> web_server;
	web_server.insert("ip", ip);
	web_server.insert("port", port);
	// 对用户名和密码进行双重加密

	// 1. 对用户名和密码进行des加密
	unsigned char encUser[1024] = {0};
	int encUserLen;

	int ret = DesEnc((unsigned char*)(user.toUtf8().data()), user.toUtf8().length(), encUser, &encUserLen);
	if (ret != 0) {
		qDebug() << "用户名des加密失败";
		return;
	}

	unsigned char encPasswd[1024] = {0};
	int encPasswdLen;

	ret = DesEnc((unsigned char*)(pwd.toUtf8().data()), pwd.toUtf8().length(), encPasswd, &encPasswdLen);
	if (ret != 0) {
		qDebug() << "密码des加密失败";
		return;
	}
	// 2. 对加密后的数据进行BASE64加密

	QByteArray base64User = QByteArray((char*)encUser, encUserLen).toBase64();
	QByteArray base64Passwd = QByteArray((char*)encPasswd, encPasswdLen).toBase64();

	// 3. 写入数据
	QMap<QString, QVariant> login;
	login.insert("user", base64User);
	login.insert("pwd", base64Passwd);
	if (isRemember) {
		login.insert("remember", "yes");
	} else {
		login.insert("remember", "no");
	}

	QMap<QString, QVariant> json;
	json.insert("login", login);
	json.insert("web_server", web_server);

	QJsonDocument jsonDoc = QJsonDocument::fromVariant(json);

	// 保存到文件
    QFile file(path);
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "文件打开失败";
	} else {
		// 开始写文件
		file.write(jsonDoc.toJson());
		file.close();
	}
}

void Common::writeWebInfo(QString ip, QString port, QString path)
{
	// web_server信息
	QMap<QString, QVariant> web_server;
	web_server.insert("ip", ip);
	web_server.insert("port", port);

	// login info信息
    QString user = getConfigValue("login", "user");
    QString pwd = getConfigValue("login", "pwd");
    QString remember = getConfigValue("login", "remember");

	QMap<QString, QVariant> login;
	login.insert("user", user);
	login.insert("pwd", pwd);
	login.insert("remember", remember);

	QMap<QString, QVariant> json;
	json.insert("web_server", web_server);
	json.insert("login", login);

	QJsonDocument jsonDoc = QJsonDocument::fromVariant(json);

	if (jsonDoc.isNull()) {
		qDebug() << "QJsonDocument::fromVariant()错误";
	}

    QFile file(path);
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "打开文件失败";
		return;
	}
	
	file.write(jsonDoc.toJson());
	if (file.isOpen()) {
		file.close();
	}
}

QString Common::getConfigValue(QString title, QString key, QString path)
{
	// 1. 读取文件
    QFile file(path);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "打开配置文件失败";
		return "";
	}
	QByteArray arr = file.readAll();
	file.close();

	// 2. 解析配置文件
	QJsonParseError err;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(arr, &err);
	if (err.error == QJsonParseError::NoError) {
		if (jsonDoc.isNull() || jsonDoc.isEmpty()) {
		qDebug() << "json doc is null or is empty";
		return "";
		}	
		if (jsonDoc.isObject()) {
			QJsonObject rootObj =  jsonDoc.object();
			QJsonValue titleValue = rootObj.value(title);
			if (titleValue.type() == QJsonValue::Object) {
				QJsonObject titleObj = titleValue.toObject();
				QStringList list = titleObj.keys();
				for (int i = 0; i < list.size(); ++i) {
					QString keyTemp = list.at(i);
					if (keyTemp == key) {
						return titleObj.value(key).toString();
					}
				}
			}
		}
	} else {
        LOG_ERROR(err.errorString());
	}
	return "";
}

QString Common::getStrMD5(QString str)
{
	QByteArray arr;
	arr = QCryptographicHash::hash(str.toLocal8Bit(), QCryptographicHash::Md5);

	return arr.toHex();
}

QString Common::getFileMd5(QString filePath)
{
    QFile localFile(filePath);

    if (!localFile.open(QFile::ReadOnly))
    {
        qDebug() << "file open error.";
        return 0;
    }

    QCryptographicHash ch(QCryptographicHash::Md5);

    quint64 totalBytes = 0;
    quint64 bytesWritten = 0;
    quint64 bytesToWrite = 0;
    quint64 loadSize = 1024 * 4;
    QByteArray buf;

    totalBytes = localFile.size();
    bytesToWrite = totalBytes;

    while (1)
    {
        if(bytesToWrite > 0)
        {
            buf = localFile.read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        }
        else
        {
            break;
        }

        if(bytesWritten == totalBytes)
        {
            break;
        }
    }

    if (localFile.isOpen()) {
        localFile.close();
    }
    QByteArray md5 = ch.result();
    return md5.toHex();
}

QString Common::getBoundary()
{
    //随机生成16个字符
    char randoms[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

    int len = strlen(randoms);
    QString temp(len, ' ');
    for (int i=0; i<16; i++) {
        int rand = QRandomGenerator::global()->bounded(0, len);
        temp[i] = randoms[rand];
    }

    qDebug() << "temp:" << temp;

    //QString randText = "DQAR0QX1ojAyzAre";  //0-9a-zA-Z
    QString boundary = "------WebKitFormBoundary" + temp;

    return boundary;
}

QNetworkAccessManager *Common::getNetworkAccessManager()
{
    return m_manager;
}

void Common::getFileTypeList()
{
    QDir dir(FILE_TYPE_DIR);
    if (!dir.exists()) {
        dir.mkpath(FILE_TYPE_DIR);
        qDebug() << FILE_TYPE_DIR << "创建成功";
    }
    /*
            QDir::Dirs      列出目录；
            QDir::AllDirs   列出所有目录，不对目录名进行过滤；
            QDir::Files     列出文件；
            QDir::Drives    列出逻辑驱动器名称，该枚举变量在Linux/Unix中将被忽略；
            QDir::NoSymLinks        不列出符号链接；
            QDir::NoDotAndDotDot    不列出文件系统中的特殊文件.及..；
            QDir::NoDot             不列出.文件，即指向当前目录的软链接
            QDir::NoDotDot          不列出..文件；
            QDir::AllEntries        其值为Dirs | Files | Drives，列出目录、文件、驱动器及软链接等所有文件；
            QDir::Readable      列出当前应用有读权限的文件或目录；
            QDir::Writable      列出当前应用有写权限的文件或目录；
            QDir::Executable    列出当前应用有执行权限的文件或目录；
            Readable、Writable及Executable均需要和Dirs或Files枚举值联合使用；
            QDir::Modified      列出已被修改的文件，该值在Linux/Unix系统中将被忽略；
            QDir::Hidden        列出隐藏文件；
            QDir::System        列出系统文件；
            QDir::CaseSensitive 设定过滤器为大小写敏感。
        */
    dir.setFilter(QDir::Files | QDir::NoDot | QDir::NoDotDot | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    dir.setSorting(QDir::Size); //排序

    //遍历文件夹(conf/fileType)
    QFileInfoList fileInfoList = dir.entryInfoList();
    for (int i=0;i<fileInfoList.size(); i++) {
        QFileInfo fileInfo = fileInfoList.at(i);
        m_fileTypeList.append(fileInfo.fileName());
    }
}

QString Common::getFileType(QString fileTypeName)
{
    if (m_fileTypeList.contains(fileTypeName))
    {
        //如果能找到
        return fileTypeName;
    } else {
        return "other.png";
    }
}

void Common::writeRecord(QString user, QString fileName, QString code, QString path)
{
    // conf/record/milo.txt
    QString userFilePath = QString("%1/%2.txt").arg(path).arg(user);
    qDebug() << "userFilePath:" << userFilePath;

    QDir dir(path);
    if (!dir.exists()) {
        //目录不存在,创建目录
        if (dir.mkpath(path)) {
            qDebug() << "目录创建成功";
        } else {
            qDebug() << "目录创建失败";
        }
    }

    QByteArray array;
    QFile file(userFilePath);
    if (file.exists()) {
        //如果存在, 先读取文件原来的内容

        if (!file.open(QIODevice::ReadOnly)) {
            //如果打开失败
            qDebug() << "file.open(QIODevice::ReadOnly) err";
            if (file.isOpen()) {
                file.close();
            }
            return;
        }

        array = file.readAll();

        if (file.isOpen()) {
            file.close();
        }
    }

    if (!file.open(QIODevice::WriteOnly)) {
        //如果打开失败
        qDebug() << "file.open(QIODevice::WriteOnly) err";
        if (file.isOpen()) {
            file.close();
        }
        return;
    }

    //记录写入文件
    //xxx.jpg   2020/11/20 16:02:01 上传成功

    QDateTime time = QDateTime::currentDateTime(); //获取系统当前时间
    QString timeStr = time.toString("yyyy/MM/dd hh:mm:ss"); //时间格式化
    QString actionString = getActionStrring(code);

    //记录到文件的内容
    QString str = QString("%1\t%2\t%3\r\n").arg(fileName).arg(timeStr).arg(actionString);
    qDebug() << "str:" << str;

    //toLocal8Bit转为本地字符串
    file.write(str.toLocal8Bit());
    if (!array.isEmpty()) {
        //读取到文件内容不为空的时候
        file.write(array);
    }

    if (file.isOpen()) {
        file.close();
    }
}

QString Common::getActionStrring(QString code)
{
    /*
    005：上传的文件已存在
    006: 秒传成功
    007: 秒传失败
    008: 上传成功
    009: 上传失败
    090: 下载成功
    091: 下载失败
    */
    QString str;
    if (code == "005") {
        str = "上传的文件已存在";
    } else if (code == "006") {
        str = "秒传成功";
    } else if (code == "007") {
        str = "秒传失败";
    } else if (code == "008") {
        str = "上传成功";
    } else if (code == "009") {
        str = "上传失败";
    } else if (code == "090") {
        str = "下载成功";
    } else if (code == "091") {
        str = "下载失败";
    }

    return str;
}

void Common::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}
