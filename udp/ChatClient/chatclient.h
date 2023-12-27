#ifndef CHATCLIENT_H
#define CHATCLIENT_H
#include "../common/Proto.h"
#include <QWidget>
#include <QEvent>
#pragma comment(lib,"Ws2_32.lib")
Q_DECLARE_METATYPE(CPackge)
namespace Ui {
class ChatClient;
}

class DataEventoffline :public QEvent
{
public:
    static const QEvent::Type type = static_cast<QEvent::Type>(QEvent::User + 4);
    explicit DataEventoffline(CPackge pck)
        :QEvent(type), m_data(pck)
    {
    }
    QString data()
    {
        return QString::fromUtf8(m_data.m_ci.m_szName);
    }
    CPackge getEventPck()
    {
        return m_data;
    }
    QString getmsg()
    {
        return QString::fromUtf8(m_data.m_szMsg);
    }
private:
    CPackge m_data;
};


class DataEventPrivaet :public QEvent
{
public:
    static const QEvent::Type type = static_cast<QEvent::Type>(QEvent::User + 3);
    explicit DataEventPrivaet(CPackge pck)
        :QEvent(type), m_data(pck)
    {
    }
    QString data()
    {
        return QString::fromUtf8(m_data.m_ci.m_szName);
    }
    CPackge getEventPck()
    {
        return m_data;
    }
    QString getmsg()
    {
        return QString::fromUtf8(m_data.m_szMsg);
    }
private:
    CPackge m_data;
};
class DataEvent :public QEvent
{
public:
    static const QEvent::Type type= static_cast<QEvent::Type>(QEvent::User+1);
    explicit DataEvent(CPackge pck)
        :QEvent(type),m_data(pck)
    {
    }
    QString data()
    {
       return QString::fromUtf8(m_data.m_ci.m_szName);
    }
    CPackge getEventPck()
    {
        return m_data;
    }
private:
    CPackge m_data;
};

/*Ⱥ����Ϣ�¼�*/
class DataEventPublic :public QEvent
{
public:
    static const QEvent::Type type = static_cast<QEvent::Type>(QEvent::User + 2);
    explicit DataEventPublic(char* data,char *name)
        :QEvent(type)
    {
        m_data = QString::fromUtf8(data);
        m_name = QString::fromUtf8(name);
    }
    QString data()
    {
        return m_data;
    }
    QString name()
    {
        return m_name;
    }
private:
    QString m_data;
    QString m_name;
};






class ChatClient : public QWidget
{
    Q_OBJECT

public:
    explicit ChatClient(QWidget *parent = nullptr);
    ~ChatClient();

private:
    Ui::ChatClient *ui;
    SOCKET m_sockClient;
    sockaddr_in m_siServer;//�洢����˵Ķ˿ں͵�ַ
    ClientInfo m_in; 
    HANDLE m_hThread;//���߳̽�������
    bool m_oerwordthread;/*�˳��߳�*/
private:
    void online();//����
    void onpublic();//Ⱥ��
    void onprivate();//˽��
    void onoffline();//����

private:
   static DWORD WINAPI WorkRecvThreadProc(LPVOID IpParameter);//���ڻ�ȡ���ݻ������̣߳���������ʹ�ÿ������߳�ȥ��ȡ���������
protected:
    void customEvent(QEvent* event)override;

};

#endif // CHATCLIENT_H
