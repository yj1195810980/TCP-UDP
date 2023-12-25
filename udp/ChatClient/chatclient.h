#ifndef CHATCLIENT_H
#define CHATCLIENT_H
#include "../common/Proto.h"
#include <QWidget>
#include <QEvent>
#pragma comment(lib,"Ws2_32.lib")

namespace Ui {
class ChatClient;
}

class DataEvent :public QEvent
{
public:
    static const QEvent::Type type= static_cast<QEvent::Type>(QEvent::User+1);
    explicit DataEvent(char* data)
        :QEvent(type)
    {
        m_data = QString::fromUtf8(data);
    }
    QString data()
    {
        return m_data;
    }
private:
    QString m_data;
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
private:
    void online();//����
    void onpublic();//Ⱥ��
    void onprivate();//˽��

private:
   static DWORD WINAPI WorkRecvThreadProc(LPVOID IpParameter);//���ڻ�ȡ���ݻ������̣߳���������ʹ�ÿ������߳�ȥ��ȡ���������
protected:
    void customEvent(QEvent* event)override;

};

#endif // CHATCLIENT_H
