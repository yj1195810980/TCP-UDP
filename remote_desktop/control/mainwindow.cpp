#include <QMessageBox>
#include <QPixmap>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(ui->action, &QAction::triggered, this, &MainWindow::OnStatcControl);//启动控制
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
		msgBox.setText("工作线程启动失败");
		msgBox.exec();
		return;
	}
}

void MainWindow::OnControlScreen()
{
	//启动远程桌面
	PackageHead hdr{ R2C_SCREEN, 0 };
	SendPackage(m_sockControl, hdr, NULL);//发送数据要求对方发送屏幕数据给我

}

bool MainWindow::RecvPackage(IN SOCKET sock, OUT PackageHead& hdr, OUT LPBYTE& pDataBuf)
{
	//收包头 如果本地可以直接收4个DWORD长度，但是如果远程的话，可能一次性收不到DWORD，所以这里循环收包
	int nRecvLen = 0;			//收到的包头长度
	DWORD dwDataLen;			//包的数据长度
	while (nRecvLen < sizeof(DWORD))
	{
		int nRet = recv(sock, (char*)&dwDataLen + nRecvLen, sizeof(dwDataLen) - nRecvLen, 0);
		nRecvLen += nRet;
	}
	hdr.m_dwDataLen = dwDataLen;




	//申请数据缓冲区
	if (hdr.m_dwDataLen == 0)
	{
		return TRUE;
	}

	pDataBuf = new BYTE[hdr.m_dwDataLen];
	if (pDataBuf == NULL)
	{
		return false;
	}

	//收数据
	nRecvLen = 0;
	while (nRecvLen < hdr.m_dwDataLen)//如果接收的数据小于包头标明的长度
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
	//发送包头
	int nRet = send(sock, (char*)&hdr, sizeof(hdr), 0);
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}


	//发送数据
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

	MainWindow* pThis = (MainWindow*)IpParam;//强转，拿到this指针

	/*创建套接字*/
	pThis->m_sockListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pThis->m_sockListen == INVALID_SOCKET)
	{
		QMessageBox msgBox;
		msgBox.setText("创建套接字失败");
		msgBox.exec();
		return 0;
	}

	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(0x2710);//大尾转小尾，本机字节序转网络字节序
	si.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
	int nRet = ::bind(pThis->m_sockListen, (sockaddr*)&si, sizeof(si));
	if (nRet == SOCKET_ERROR)
	{
		QMessageBox msgBox;
		msgBox.setText("bind失败");
		msgBox.exec();
		return 0;
	}

	/*监听*/
	nRet = listen(pThis->m_sockListen, SOMAXCONN);
	if (nRet == SOCKET_ERROR)
	{
		QMessageBox msgBox;
		msgBox.setText("监听失败");
		msgBox.exec();
		return 0;
	}

	sockaddr_in siClient;
	int nLen = sizeof(siClient);
	pThis->m_sockControl = accept(pThis->m_sockListen, (sockaddr*)&siClient, &nLen);
	if (pThis->m_sockControl == INVALID_SOCKET)
	{
		QMessageBox msgBox;
		msgBox.setText("等待链接失败");
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
			//将数据显示到界面上去
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

