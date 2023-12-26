#include "chatclient.h"
#include "ui_chatclient.h"
#include <iostream>
#include <QPushButton>
#include <QMessageBox>
#include <QVariant>
#include <QtGlobal>
#include <QTextStream>
#include "../common/CSockinit.h"
ChatClient::ChatClient(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::ChatClient)
{
	ui->setupUi(this);
	m_sockClient = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sockClient == INVALID_SOCKET)
	{
		QMessageBox::warning(nullptr, u8"警告", u8"套接字创建失败");
		return;
	}

	//客户端bind
	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = 0;//给的0，代表客户端自行分配一个端口
	//si.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int nRet = bind(m_sockClient, (sockaddr*)&si, sizeof(si));
	if (nRet == SOCKET_ERROR)
	{
		QMessageBox::warning(nullptr, u8"警告", u8"bind失败");
	}

	//服务端的地址和端口
	m_siServer.sin_family = AF_INET;
	m_siServer.sin_port = ::htons(0x7788);
	m_siServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


	//获取当前程序的端口
	sockaddr_in siself;
	int nLen = sizeof(siself);
	getsockname(m_sockClient, (sockaddr*)&siself, &nLen);
	m_in.m_si.sin_family = AF_INET;
	m_in.m_si.sin_port = siself.sin_port;
	m_in.m_si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//启动工作线程(接收数据)
	SECURITY_ATTRIBUTES se = {};
	se.nLength = sizeof(se);
	se.bInheritHandle = NULL;
	m_hThread = ::CreateThread(&se, 0, WorkRecvThreadProc, (LPVOID)this, 0, NULL);

	connect(ui->pushButton, &QPushButton::clicked, this, &ChatClient::online);
	connect(ui->pushButton_3, &QPushButton::clicked, this, &ChatClient::onpublic);
	connect(ui->pushButton_4, &QPushButton::clicked, this, &ChatClient::onprivate);
	connect(ui->pushButton_2, &QPushButton::clicked, this, &ChatClient::onoffline);
}

ChatClient::~ChatClient()
{
	delete ui;
	CloseHandle(m_hThread);
}

DWORD WINAPI ChatClient::WorkRecvThreadProc(LPVOID obj)
{
	while (true)
	{
		sockaddr_in siFrom = {};
		int nSizeofSi = sizeof(sockaddr_in);
		CPackge pack;//接收的是一个自定义的包
		//将接收的数据按照char类型进行存放
		auto th = (ChatClient*)obj;
		int nRet = ::recvfrom(th->m_sockClient, (char*)&pack, sizeof(pack), 0, (sockaddr*)&siFrom, &nSizeofSi);
		if (nRet == 0 || nRet == SOCKET_ERROR)
		{
			continue;
		}
		else
		{
			//收到的数据
			switch (pack.m_pt)
			{
			case PT_ONLINE:
			{
				auto th = (ChatClient*)obj;
				DataEvent* event = new DataEvent(pack);
				QApplication::postEvent(th, event);
				break;
			}
			case PT_OFFLINE:
				break;
			case PT_PUBLIC:
			{
				DataEventPublic* event = new DataEventPublic(pack.m_ci.m_szName,pack.m_szMsg);
				auto th = (ChatClient*)obj;
				QApplication::postEvent(th, event);
				break;
			}
			case PT_PRIVATE:
			{
				auto th = (ChatClient*)obj;
				DataEventPrivaet* event = new DataEventPrivaet(pack);
				QApplication::postEvent(th, event);
				break;
			}
			default:
				break;
			}
		}
	}
	return 0;
}

void ChatClient::online()
{
	//up online 
	if (ui->lineEdit->text().isEmpty())
	{
		QMessageBox::warning(nullptr, u8"警告", u8"昵称为空");
		return;
	}
	QString name = ui->lineEdit->text();


	strncpy(m_in.m_szName, name.toUtf8().constData(), sizeof(name.toUtf8().constData()));
	m_in.m_szName[sizeof(m_in.m_szName) - 1] = '\0';
	CPackge pgk(PT_ONLINE, m_in, "");
	::sendto(m_sockClient, (char*)&pgk, sizeof(pgk), 0, (sockaddr*)&m_siServer, sizeof(m_siServer));
}


void ChatClient::onpublic()
{
	if (ui->lineEdit_2->text().isEmpty())
	{
		QMessageBox::warning(nullptr, u8"警告", u8"输入消息不能为空");
		return;
	}
	QString msgData = ui->lineEdit_2->text();
	CPackge pgk(PT_PUBLIC, m_in, "");
	
	memset(pgk.m_szMsg, 0, sizeof(pgk.m_szMsg));
	
	int copyLength = qMin(msgData.toUtf8().size(), static_cast<int>(sizeof(pgk.m_szMsg)) - 1);
	strncpy(pgk.m_szMsg, msgData.toUtf8().constData(), copyLength);
	
	::sendto(m_sockClient, (char*)&pgk, sizeof(pgk), 0, (sockaddr*)&m_siServer, sizeof(m_siServer));
}

void ChatClient::customEvent(QEvent* event)
{
	//处理自定义事件
	switch (event->type())
	{
	case DataEvent::type:
	{
		/*上线消息*/
		DataEvent* dataEvent = static_cast<DataEvent*>(event);
		QString temp = dataEvent->data();
		QListWidgetItem* item = new QListWidgetItem(temp, ui->listWidget);
		auto pck = dataEvent->getEventPck();
		QVariant variant;
		variant.setValue(pck);
		item->setData(Qt::UserRole, variant);/*将数据存储在item中*/
		break;
	}
	case DataEventPublic::type:
	{
		/*群发消息*/
		DataEventPublic* dataEvent = static_cast<DataEventPublic*>(event);
		QString data = dataEvent->data();
		QString name = dataEvent->name();
		QString str;
		QString formattedString = QString(u8"名称：%1，内容：%2").arg(data).arg(name);
		ui->textBrowser->append(formattedString);
	}
	case DataEventPrivaet::type:
	{
		/*私聊消息*/
		DataEventPrivaet* dataEvent = static_cast<DataEventPrivaet*>(event);
		QString str;
		QString formattedString = QString(u8"名称：%1，内容：%2").arg(dataEvent->getEventPck().m_ci.m_szName).arg(dataEvent->getmsg());
		ui->textBrowser->append(formattedString);
	}
	default:
		break;
	}

}


void ChatClient::onprivate()
{
	if (ui->lineEdit_2->text().isEmpty())
	{
		QMessageBox::warning(nullptr, u8"警告", u8"输入消息不能为空");
		return;
	}
	auto curritem=ui->listWidget->currentItem();
	if (curritem)
	{
		QVariant stredData= curritem->data(Qt::UserRole);/*获取用户自定义的数据*/
		if (stredData.isValid())
		{
			auto pck = stredData.value<CPackge>();
			QString msgData = ui->lineEdit_2->text();
			auto ms = curritem->text();
			CPackge pgk(PT_PRIVATE, m_in, pck.m_ci,"");

			memset(pgk.m_szMsg, 0, sizeof(pgk.m_szMsg));
			int copyLength = qMin(msgData.toUtf8().size(), static_cast<int>(sizeof(pgk.m_szMsg)) - 1);
			strncpy(pgk.m_szMsg, msgData.toUtf8().constData(), copyLength);
			::sendto(m_sockClient, (char*)&pgk, sizeof(pgk), 0, (sockaddr*)&m_siServer, sizeof(m_siServer));
		}
	}
	else
	{
		QMessageBox::warning(nullptr, u8"警告", u8"未选中私聊对象");
	}
}

void ChatClient::onoffline()
{
	/*下线*/
	CPackge pgk(PT_PRIVATE, m_in,"");
	::sendto(m_sockClient, (char*)&pgk, sizeof(pgk), 0, (sockaddr*)&m_siServer, sizeof(m_siServer));
}
