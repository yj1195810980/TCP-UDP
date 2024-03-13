#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<iostream>
#include"../common/CSockinit.h"
#include"../common/proto.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace std;
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private:
	void OnStatcControl();//启动处理线程
	void OnControlScreen();//启动远程桌面

private:
	 bool RecvPackage(IN SOCKET sock, OUT PackageHead& hdr, OUT LPBYTE& pDataBuf);
	 bool SendPackage(IN SOCKET sock, IN const PackageHead& hdr, IN LPBYTE pDataBuf);
	 void OnScreen(LPBYTE pBuff);


signals:
	void upShow(LPBYTE pBuff);

private:
	Ui::MainWindow* ui;
	SOCKET m_sockListen;
	SOCKET m_sockControl;
	bool m_bWorkng = false;//退出标志
	HANDLE m_hThread;
	QWidget* m_show;
	QImage* m_image;
	static DWORD CALLBACK WorkThreadFunc(LPVOID IpParam);
};
#endif // MAINWINDOW_H
