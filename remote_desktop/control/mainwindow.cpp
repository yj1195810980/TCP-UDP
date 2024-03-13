#include <QMessageBox>
#include <QPixmap>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(ui->action, &QAction::triggered, this, &MainWindow::OnStatcControl);//��������
	connect(ui->action_3, &QAction::triggered, this, &MainWindow::OnControlScreen);
	connect(this, &MainWindow::upShow, this,&MainWindow::OnScreen);


}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::OnStatcControl()
{
	m_bWorkng = true;
	m_hThread = CreateThread(NULL, 0, WorkThreadFunc, this, 0, NULL);
	if (m_hThread == NULL)
	{
		QMessageBox msgBox;
		msgBox.setText("�����߳�����ʧ��");
		msgBox.exec();
		return;
	}
}

void MainWindow::OnControlScreen()
{
	//����Զ������
	PackageHead hdr{ R2C_SCREEN, 0 };
	SendPackage(m_sockControl, hdr, NULL);//��������Ҫ��Է�������Ļ���ݸ���

}

bool MainWindow::RecvPackage(IN SOCKET sock, OUT PackageHead& hdr, OUT LPBYTE& pDataBuf)
{
	//�հ�ͷ ������ؿ���ֱ����4��DWORD���ȣ��������Զ�̵Ļ�������һ�����ղ���DWORD����������ѭ���հ�
	int nRecvLen = 0;			//�յ��İ�ͷ����
	DWORD dwDataLen;			//�������ݳ���
	while (nRecvLen < sizeof(DWORD))
	{
		int nRet = recv(sock, (char*)&dwDataLen + nRecvLen, sizeof(dwDataLen) - nRecvLen, 0);
		nRecvLen += nRet;
	}
	hdr.m_dwDataLen = dwDataLen;




	//�������ݻ�����
	if (hdr.m_dwDataLen == 0)
	{
		return TRUE;
	}

	pDataBuf = new BYTE[hdr.m_dwDataLen];
	if (pDataBuf == NULL)
	{
		return false;
	}

	//������
	nRecvLen = 0;
	while (nRecvLen < hdr.m_dwDataLen)//������յ�����С�ڰ�ͷ�����ĳ���
	{
		int nRet = recv(sock, (char*)&pDataBuf + nRecvLen, hdr.m_dwDataLen - nRecvLen, 0);
		if (nRet == SOCKET_ERROR || nRet == 0)
		{
			delete[] pDataBuf;
			return false;
		}
		nRecvLen += nRet;
	}
	return true;
}

bool MainWindow::SendPackage(IN SOCKET sock, IN const PackageHead& hdr, IN LPBYTE pDataBuf)
{
	//���Ͱ�ͷ
	int nRet = send(sock, (char*)&hdr, sizeof(hdr), 0);
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}


	//��������
	nRet = send(sock, (char*)&pDataBuf, hdr.m_dwDataLen, 0);
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}

void MainWindow::OnScreen(LPBYTE pBuff)
{
	cout << pBuff[10] << endl;;
}

DWORD CALLBACK MainWindow::WorkThreadFunc(LPVOID IpParam)
{

	MainWindow* pThis = (MainWindow*)IpParam;//ǿת���õ�thisָ��

	/*�����׽���*/
	pThis->m_sockListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pThis->m_sockListen == INVALID_SOCKET)
	{
		QMessageBox msgBox;
		msgBox.setText("�����׽���ʧ��");
		msgBox.exec();
		return 0;
	}

	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(0x2710);//��βתСβ�������ֽ���ת�����ֽ���
	si.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
	int nRet = ::bind(pThis->m_sockListen, (sockaddr*)&si, sizeof(si));
	if (nRet == SOCKET_ERROR)
	{
		QMessageBox msgBox;
		msgBox.setText("bindʧ��");
		msgBox.exec();
		return 0;
	}

	/*����*/
	nRet = listen(pThis->m_sockListen, SOMAXCONN);
	if (nRet == SOCKET_ERROR)
	{
		QMessageBox msgBox;
		msgBox.setText("����ʧ��");
		msgBox.exec();
		return 0;
	}

	sockaddr_in siClient;
	int nLen = sizeof(siClient);
	pThis->m_sockControl = accept(pThis->m_sockListen, (sockaddr*)&siClient, &nLen);
	if (pThis->m_sockControl == INVALID_SOCKET)
	{
		QMessageBox msgBox;
		msgBox.setText("�ȴ�����ʧ��");
		msgBox.exec();
		return 0;
	}


	while (pThis->m_bWorkng)
	{
		PackageHead hdr;
		LPBYTE pBuff = NULL;
		if (!pThis->RecvPackage(pThis->m_sockControl,hdr, pBuff))
		{
			continue;
		}
		switch (hdr.m_command)
		{
		case C2R_CMD:
			break;
		case C2R_SCREEN:
		{
			//��������ʾ��������ȥ
			emit pThis->upShow(pBuff);
		}
			break;
		case C2R_FILETRAVELS:
			break;
		case C2R_FILEUPLOAD:
			break;
		case R2C_SCREEN:
			break;
		default:
			break;
		}
	

	}
	return 0;
}

