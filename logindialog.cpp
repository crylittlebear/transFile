#pragma execution_character_set("utf-8")

#include "logindialog.h"

#include "qnetworkaccessmanager.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonvalue.h"

#include "logger.h"
#include "logininfoinstance.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
	, ui(new Ui::LoginDialogClass())
{
	ui->setupUi(this);

    init();
}

LoginDialog::~LoginDialog()
{
	delete ui;
}

void LoginDialog::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		isMoving_ = true;
		position_ = event->pos();
	}
}

void LoginDialog::mouseMoveEvent(QMouseEvent* event)
{
	if (isMoving_ && (event->buttons() & Qt::LeftButton)) {
        this->move(event->globalPos() - position_);
	}
}

void LoginDialog::mouseReleaseEvent(QMouseEvent* event)
{
	if (isMoving_) {
		isMoving_ = false;
	}
}

/**
 * 登录按钮点击槽函数
*/
void LoginDialog::sltBtnLoginClicked()
{
    LOG_INFO("点击了登录按钮");
	QString userName = ui->lineEditUser->text();
	QString passwd = ui->lineEditPasswd->text();

    // 验证用户名和密码的格式是否正确
	QRegExp reg(USER_REG);
	if (!reg.exactMatch(userName)) {
		QMessageBox::warning(this, "警告", "用户名格式不正确");
		ui->lineEditUser->clear();
		ui->lineEditUser->setFocus();
		return;
	}
	reg.setPattern(PASSWD_REG);
	if (!reg.exactMatch(passwd)) {
		QMessageBox::warning(this, "警告", "密码格式不正确");
		ui->lineEditPasswd->clear();
		ui->lineEditPasswd->setFocus();
		return;
	}

	// 发送HTTP请求
	QNetworkRequest request;
    // 从配置文件中读取服务器IP和端口号信息
    QString ip = common_->getConfigValue("web_server", "ip");
    QString port = common_->getConfigValue("web_server", "port");
	QString url(QString("http://%1:%2/login").arg(ip).arg(port));
    LOG_DEBUG("登录URL为: " + url);
	request.setUrl(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	// 将文本数据封装成json

	QJsonObject paramObj;
	paramObj.insert("user", userName);
	paramObj.insert("pwd", common_->getStrMD5(passwd));

	QJsonDocument document(paramObj);

	QByteArray data = document.toJson();
    LOG_DEBUG("登录信息为：" + data);
    // 发送HTTP请求并通过QNetworkReply接收
    QNetworkReply* reply = networkManager_->post(request, data);

	// 读取服务器返回的数据
	connect(reply, &QNetworkReply::readyRead, this, [=](){
		// 读数据
		QByteArray data = reply->readAll();
        LOG_DEBUG("接收到来自服务器的登录响应为：" + data);

		// 解析返回的数据
		QJsonParseError err;
		QJsonDocument document = QJsonDocument::fromJson(data, &err);
		if (err.error != QJsonParseError::NoError) {
			QMessageBox::critical(this, "错误", "解析服务器返回消息失败");
		} else {
			QJsonObject rootObj = document.object();
			QJsonValue codeValue = rootObj.value("code");
			if (codeValue.type() == QJsonValue::String) {
				QString msg = codeValue.toString();
				if ("000" == msg) {
					QMessageBox::information(this, "消息", "登录成功");
					// 将用户信息保存到cfg.json文件
					bool isCheck = ui->checkBox->isChecked();
                    // 如果没有设置保存登录信息，清除密码输入框内容
					if (isCheck == false) {
						ui->lineEditPasswd->setText("");
					}
					// 保存配置信息
					common_->writeLoginInfo(userName, passwd, isCheck);

                    //获取token
                    QJsonValue tokenValue = rootObj.value("token");
                    saveLoginInfoData(userName, tokenValue.toString(), ip, port);

					// 显示主窗口，隐藏登录窗口
                    this->hide();
                    mainWindow_->show();
                    emit sigLoginSuccess(userName);

				} else if ("001" == msg) {
					QMessageBox::critical(this, "错误", "登录失败");
				}
			}
		}
	});
}

/**
 * 注册按钮点击槽函数
*/
void LoginDialog::sltBtnRegClicked()
{
	QString user = ui->lineEditRegUser->text();
	QString nickName = ui->lineEditRegNickName->text();
	QString passwd = ui->lineEditRegPasswd->text();
	QString passwdConfirm = ui->lineEditRegPasswdConfirm->text();
	QString phone = ui->lineEditRegPhone->text();
	QString email = ui->lineEditRegEmail->text();
	// 校验输入

	// 用户名校验
	QRegExp regName(USER_REG);
	if (!regName.exactMatch(user)) {
		QMessageBox::warning(this, "警告", "用户名格式不正确");
		ui->lineEditRegUser->clear();
		ui->lineEditRegUser->setFocus();
		return;
	}

	// 昵称校验
	if (!regName.exactMatch(nickName)) {
		QMessageBox::warning(this, "警告", "昵称格式不正确");
		ui->lineEditRegNickName->clear();
		ui->lineEditRegNickName->setFocus();
		return;
	}

	// 密码校验

	// 首先检查两次密码是否一样
	if (passwd != passwdConfirm) {
		QMessageBox::warning(this, "警告", "两次密码输入不一致");
		ui->lineEditRegPasswd->setFocus();
		return;
	}

	QRegExp regPasswd(PASSWD_REG);

	if (!regPasswd.exactMatch(passwd)) {
		QMessageBox::warning(this, "警告", "密码格式不正确");
		ui->lineEditRegPasswd->clear();
		ui->lineEditRegPasswd->setFocus();
		return;
	}

	if (!regPasswd.exactMatch(passwdConfirm)) {
		QMessageBox::warning(this, "警告", "确认密码格式不正确");
		ui->lineEditRegPasswdConfirm->clear();
		ui->lineEditRegPasswdConfirm->setFocus();
		return;
	}

	// 电话号码校验
	QRegExp regPhone(PHONE_REG);

	if (!regPhone.exactMatch(phone)) {
		QMessageBox::warning(this, "警告", "电话号码格式不正确");
		ui->lineEditRegPhone->clear();
		ui->lineEditRegPhone->setFocus();
		return;
	}

	// 邮箱校验
	QRegExp regMail(EMAIL_REG);

	if (!regMail.exactMatch(email)) {
		QMessageBox::warning(this, "警告", "邮箱格式不正确");
		ui->lineEditRegEmail->clear();
		ui->lineEditRegEmail->setFocus();
		return;
	}

	// 发送HTTP请求
	QNetworkRequest request;
    QString ip = common_->getConfigValue("web_server", "ip");
    QString port = common_->getConfigValue("web_server", "port");
	QString url(QString("http://%1:%2/reg").arg(ip).arg(port));
    LOG_DEBUG("注册URL为：" + url);
	request.setUrl(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	// 将文本数据封装成json

	QJsonObject paramObj;
	paramObj.insert("email", email);
	paramObj.insert("userName", user);
	paramObj.insert("phone", phone);
	paramObj.insert("nickName", nickName);
	paramObj.insert("firstPwd", common_->getStrMD5(passwd));

	QJsonDocument document(paramObj);

	QByteArray data = document.toJson();

    QNetworkReply* reply = networkManager_->post(request, data);

	// 读取服务器返回的数据
	connect(reply, &QNetworkReply::readyRead, this, [=](){
		// 读数据
		QByteArray data = reply->readAll();
        LOG_DEBUG("接收到来自服务器的注册响应为：" + data);

		// 解析返回的数据
		QJsonParseError err;
		QJsonDocument document = QJsonDocument::fromJson(data, &err);
		if (err.error != QJsonParseError::NoError) {
			QMessageBox::critical(this, "错误", "解析服务器返回消息失败");
		} else {
			QJsonObject rootObj = document.object();
			QJsonValue codeValue = rootObj.value("code");
			if (codeValue.type() == QJsonValue::String) {
				QString msg = codeValue.toString();
				if ("002" == msg) {
					QMessageBox::information(this, "消息", "注册成功");
					ui->stackedWidget->setCurrentWidget(ui->pageLogin);
				} else if ("003" == msg) {
					QMessageBox::information(this, "消息", "用户已存在");
				} else if ("004" == msg) {
					QMessageBox::critical(this, "错误", "注册失败");
				}
			}
		}
	});
}

void LoginDialog::slgBtnRegBacktoLoginClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
}

void LoginDialog::sltBtnServerSetClicked()
{
	QString ip = ui->lineEditIP->text();
	QString port = ui->lineEditPort->text();
    LOG_DEBUG("输入框内的IP为" + ip + "，端口号为：" + port);

	// 验证IP和地址的格式
	QRegExp ipExp(IP_REG);

	if (!ipExp.exactMatch(ip)) {
		QMessageBox::warning(this, "警告", "IP地址格式错误");
		return;
	}

	QRegExp portExp(PORT_REG);
	if (!portExp.exactMatch(port)) {
		QMessageBox::warning(this, "警告", "端口格式错误");
		return;
	}

	// 将IP和端口信息保存到config文件
	ui->stackedWidget->setCurrentWidget(ui->pageLogin);
	common_->writeWebInfo(ip, port);
}

void LoginDialog::slgBtnRegFontClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageRegister);
}

void LoginDialog::init()
{
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowIcon(QIcon(":/img/cloud.png"));

    isMoving_ = false;

    common_ = Common::instance();
    networkManager_ = Common::instance()->getNetworkAccessManager();

    mainWindow_ = new MainWindow;

    ui->stackedWidget->setCurrentWidget(ui->pageLogin);

    // 设置输入框内的ICON图标
    ui->lineEditUser->setIconPath(":/img/user.png");
    ui->lineEditPasswd->setIconPath(":/img/lock.png");
    // 建立信号槽连接
    buildConnections();
    // 从cfg.json配置文件中读取信息到界面上
    readConfigFile();
}

void LoginDialog::buildConnections()
{
    // 连接按钮与响应的StackedWidget对应的页面
    connect(ui->widgetTitle, &LoginTitle::sigBtnConfigClicked, [=](){
        ui->stackedWidget->setCurrentWidget(ui->pageConfig);
    });

    connect(ui->widgetTitle, &LoginTitle::sigBtnMinClicked, [=](){
        this->showMinimized();
    });

    connect(ui->widgetTitle, &LoginTitle::sigBtnCloseClicked, [=](){
        if (ui->stackedWidget->currentWidget() == ui->pageConfig) {
            ui->stackedWidget->setCurrentWidget(ui->pageLogin);
        } else {
            this->close();
        }
    });
    // 连接登录、注册、设置IP端口按钮的信号和槽函数
    connect(ui->tBtnRegFont, &QToolButton::clicked, this, &LoginDialog::slgBtnRegFontClicked);
    connect(ui->tBtnBacktoLogin, &QToolButton::clicked, this, &LoginDialog::slgBtnRegBacktoLoginClicked);
    connect(ui->toolButtonLogin, &QToolButton::clicked, this, &LoginDialog::sltBtnLoginClicked);
    connect(ui->tBtnReg, &QToolButton::clicked, this, &LoginDialog::sltBtnRegClicked);
    connect(ui->tBtnServerSet, &QToolButton::clicked, this, &LoginDialog::sltBtnServerSetClicked);

    // 设置连接主界面注销登录信号
    connect(mainWindow_, &MainWindow::sigLogout, [=]() {
        mainWindow_->hide();
        this->show();
    });
}

void LoginDialog::readConfigFile()
{
    QString user = common_->getConfigValue("login", "user");
    QString passwd = common_->getConfigValue("login", "pwd");
    QString remember = common_->getConfigValue("login", "remember");
	if (remember == "yes") {
		// 记住密码
		// 1. base64解密
		QByteArray pwdTemp = QByteArray::fromBase64(passwd.toLocal8Bit());
		// 2. des解密
		unsigned char pwdDec[1024];
		int pwdDecLen = 0;
		int ret = DesDec((unsigned char*)pwdTemp.data(), pwdTemp.size(), pwdDec, &pwdDecLen);
        QString pwd;
        if (ret == 0) {
            pwd = QString::fromLocal8Bit((const char*)(pwdDec), pwdDecLen);
            LOG_INFO("密码解密成功");
        } else {
            LOG_ERROR("密码解密失败");
        }
		ui->checkBox->setChecked(true);
		ui->lineEditPasswd->setText(pwd);
	} else {
		ui->checkBox->setChecked(false);
		ui->lineEditPasswd->setText("");
	}

	QByteArray userTemp = QByteArray::fromBase64(user.toLocal8Bit());
	// 2. des解密
	unsigned char userDec[1024];
	int userDecLen = 0;
	int ret = DesDec((unsigned char*)userTemp.data(), userTemp.size(), userDec, &userDecLen);
    QString userName;
    if (ret == 0) {
        userName = QString::fromLocal8Bit((const char*)(userDec), userDecLen);
        LOG_INFO("用户名解密成功");
    } else {
        LOG_ERROR("用户名解密失败");
    }
	ui->lineEditUser->setText(userName);


    QString ip = common_->getConfigValue("web_server", "ip");
    QString port = common_->getConfigValue("web_server", "port");
	if (ip != "") {
		ui->lineEditIP->setText(ip);
	}
	if (port != "") {
		ui->lineEditPort->setText(port);
	}
}

void LoginDialog::saveLoginInfoData(QString userName, QString token, QString ip, QString port)
{
    //跳转到其他页面
    //保存数据, token, user, ip, 端口
    //除了登录外：每一个请求都需要校验token,每一个请求都需要带token
    LoginInfoInstance *loginInfo = LoginInfoInstance::instance();

    loginInfo->setUser(userName);
    loginInfo->setToken(token);
    loginInfo->setIp(ip);
    loginInfo->setPort(port);
    LOG_INFO(QString("成功保存信息到LoginInfoInstance类中，userName:%1，token:%2，ip:%3，port:%4").arg(userName).arg(token).arg(ip).arg(port));
}
