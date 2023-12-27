#include "chatclient.h"
#include "ui_chatclient.h"
#include <iostream>
#include <QPushButton>
#include <QMessageBox>
#include <QVariant>
#include <QtGlobal>
#include <QTextStream>
#include<qdebug.h>
#include "../common/CSockinit.h"
ChatClient::ChatClient(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::ChatClient)
{
	ui->setupUi(this);
	m_sockClient = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sockClient == INVALID_SOCKET)
	{
		QMessageBox::warning(nullptr, u8"����", u8"�׽��ִ���ʧ��");
		return;
	}

	//�ͻ���bind
	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = 0;//����0������ͻ������з���һ���˿�
	//si.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int nRet = bind(m_sockClient, (sockaddr*)&si, sizeof(si));
	if (nRet == SOCKET_ERROR)
	{
		QMessageBox::warning(nullptr, u8"����", u8"bindʧ��");
	}

	//����˵ĵ�ַ�Ͷ˿�
	m_siServer.sin_family = AF_INET;
	m_siServer.sin_port = ::htons(0x7788);
	m_siServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


	//��ȡ��ǰ����Ķ˿�
	sockaddr_in siself;
	int nLen = sizeof(siself);
	getsockname(m_sockClient, (sockaddr*)&siself, &nLen);
	m_in.m_si.sin_family = AF_INET;
	m_in.m_si.sin_port = siself.sin_port;
	m_in.m_si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//���������߳�(��������)
	SECURITY_ATTRIBUTES se = {};
	se.nLength = sizeof(se);
	se.bInheritHandle = NULL;
	m_oerwordthread = true;
	m_hThread = ::CreateThread(&se, 0, WorkRecvThreadProc, (LPVOID)this, 0, NULL);

	connect(ui->pushButton, &QPushButton::clicked, this, &ChatClient::online);
	connect(ui->pushButton_3, &QPushButton::clicked, this, &ChatClient::onpublic);
	connect(ui->pushButton_4, &QPushButton::clicked, this, &ChatClient::onprivate);
	connect(ui->pushButton_2, &QPushButton::clicked, this, &ChatClient::onoffline);

	ui->pushButton_2->setEnabled(false);
	ui->pushButton_3->setEnabled(false);
	ui->pushButton_4->setEnabled(false);

}

ChatClient::~ChatClient()
{
	delete ui;
	CloseHandle(m_hThread);
}

DWORD WINAPI ChatClient::WorkRecvThreadProc(LPVOID obj)
{
	auto th = (ChatClient*)obj;
	while (th->m_oerwordthread)
	{
		sockaddr_in siFrom = {};
		int nSizeofSi = sizeof(sockaddr_in);
		CPackge pack;//���յ���һ���Զ���İ�
		//�����յ����ݰ���char���ͽ��д��
		auto th = (ChatClient*)obj;
		int nRet = ::recvfrom(th->m_sockClient, (char*)&pack, sizeof(pack), 0, (sockaddr*)&siFrom, &nSizeofSi);
		if (nRet == 0 || nRet == SOCKET_ERROR)
		{
			continue;
		}
		else
		{
			//�յ�������
			switch (pack.m_pt)
			{
			case PT_ONLINE:
			{
				DataEvent* event = new DataEvent(pack);
				QApplication::postEvent(th, event);
				break;
			}
			case PT_OFFLINE:
			{
				DataEventoffline* event = new DataEventoffline(pack);
				QApplication::postEvent(th, event);
				break;
			}
			case PT_PUBLIC:
			{
				DataEventPublic* event = new DataEventPublic(pack.m_ci.m_szName, pack.m_szMsg);
				QApplication::postEvent(th, event);
				break;
			}
			case PT_PRIVATE:
			{
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
	ui->pushButton_2->setEnabled(true);
	ui->pushButton_3->setEnabled(true);
	ui->pushButton_4->setEnabled(true);
	ui->pushButton->setEnabled(false);


	//up online 
	if (ui->lineEdit->text().isEmpty())
	{
		QMessageBox::warning(nullptr, u8"����", u8"�ǳ�Ϊ��");
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
		QMessageBox::warning(nullptr, u8"����", u8"������Ϣ����Ϊ��");
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
	//�����Զ����¼�
	switch (event->type())
	{
	case DataEvent::type:
	{
		/*������Ϣ*/
		DataEvent* dataEvent = static_cast<DataEvent*>(event);
		QString temp = dataEvent->data();
		QListWidgetItem* item = new QListWidgetItem(temp, ui->listWidget);
		auto pck = dataEvent->getEventPck();
		QVariant variant;
		variant.setValue(pck);
		item->setData(Qt::UserRole, variant);/*�����ݴ洢��item��*/
		break;
	}
	case DataEventPublic::type:
	{
		/*Ⱥ����Ϣ*/
		DataEventPublic* dataEvent = static_cast<DataEventPublic*>(event);
		QString data = dataEvent->data();
		QString name = dataEvent->name();
		QString str;
		QString formattedString = QString(u8"���ƣ�%1�����ݣ�%2").arg(data).arg(name);
		ui->textBrowser->append(formattedString);
		break;
	}
	case DataEventPrivaet::type:
	{
		/*˽����Ϣ*/
		DataEventPrivaet* dataEvent = static_cast<DataEventPrivaet*>(event);
		QString str;
		QString formattedString = QString(u8"���ƣ�%1�����ݣ�%2").arg(dataEvent->getEventPck().m_ci.m_szName).arg(dataEvent->getmsg());
		ui->textBrowser->append(formattedString);
		break;
	}
	case DataEventoffline::type:
	{
		/*�յ��������ͻ������ߵ���Ϣ*/
		DataEventoffline* dataEvent = static_cast<DataEventoffline*>(event);
		QString str;
		QString formattedString = QString(u8"���ƣ�%1��������").arg(dataEvent->getEventPck().m_ci.m_szName);
		ui->textBrowser->append(formattedString);

		for (int i = 0; i < ui->listWidget->count(); i++)
		{
			QListWidgetItem* temp = ui->listWidget->item(i);
			QVariant stredData = temp->data(Qt::UserRole);/*��ȡ�û��Զ��������*/
			if (stredData.isValid())
			{
				auto pck = stredData.value<CPackge>();
				if (pck.m_ci == dataEvent->getEventPck().m_ci)
				{
					ui->listWidget->takeItem(i);
					delete temp;
				}
			}
		}
		break;
	}
	default:
		break;
	}
}


void ChatClient::onprivate()
{
	if (ui->lineEdit_2->text().isEmpty())
	{
		QMessageBox::warning(nullptr, u8"����", u8"������Ϣ����Ϊ��");
		return;
	}
	auto curritem = ui->listWidget->currentItem();
	if (curritem)
	{
		QVariant stredData = curritem->data(Qt::UserRole);/*��ȡ�û��Զ��������*/
		if (stredData.isValid())
		{
			auto pck = stredData.value<CPackge>();
			QString msgData = ui->lineEdit_2->text();
			auto ms = curritem->text();
			CPackge pgk(PT_PRIVATE, m_in, pck.m_ci, "");

			memset(pgk.m_szMsg, 0, sizeof(pgk.m_szMsg));
			int copyLength = qMin(msgData.toUtf8().size(), static_cast<int>(sizeof(pgk.m_szMsg)) - 1);
			strncpy(pgk.m_szMsg, msgData.toUtf8().constData(), copyLength);
			::sendto(m_sockClient, (char*)&pgk, sizeof(pgk), 0, (sockaddr*)&m_siServer, sizeof(m_siServer));
		}
	}
	else
	{
		QMessageBox::warning(nullptr, u8"����", u8"δѡ��˽�Ķ���");
	}
}

void ChatClient::onoffline()
{
	ui->pushButton_2->setEnabled(false);
	ui->pushButton_3->setEnabled(false);
	ui->pushButton_4->setEnabled(false);
	ui->pushButton->setEnabled(true);

	/*����*/
	CPackge pgk(PT_OFFLINE, m_in, "");
	::sendto(m_sockClient, (char*)&pgk, sizeof(pgk), 0, (sockaddr*)&m_siServer, sizeof(m_siServer));

	closesocket(m_sockClient);/*���ߺ�ر��׽���*/
	m_oerwordthread = false;/*�ر��̻߳�ȡ����������*/

	/*��������б�*/
	for (int i = 0; i < ui->listWidget->count(); ++i)
	{
		auto item=ui->listWidget->item(i);
		ui->listWidget->takeItem(i);
		delete item;
	}
	ui->listWidget->clear();


}
