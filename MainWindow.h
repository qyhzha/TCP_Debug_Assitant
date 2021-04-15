#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <cassert>

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QWidget>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>

#include "QLightLabel.h"

class MainWindow : public QMainWindow
{
        Q_OBJECT

    protected:
        struct tcpClientRecord : public QObject
        {
            QString hostIpStr;
            QString buttonStr;
            bool lightState;
            bool sendFileEnable;
        };

        struct tcpServerRecord : public QObject
        {
            QString buttonStr;
            bool lightState;
            bool sendFileEnable;
        };

    protected:
        QAction *m_sendFileAction;
        QAction *m_receiveToFile;
        QAction *ascii;
        QAction *utf_8;
        QRadioButton *m_tcpClientButton;
        tcpClientRecord m_tcpClientRecord;
        QTcpSocket *m_tcpClient;
        QRadioButton *m_tcpServerButton;
        tcpServerRecord m_tcpServerRecord;
        QTcpServer *m_tcpServer;
        QRadioButton *m_udp;
        QLineEdit *m_hostIp;
        QSpinBox *hostPort;
        QLightLabel *m_light;
        QPushButton *m_button;
        QPlainTextEdit *m_receiveBuffer;
        QPlainTextEdit *m_sendBuffer;

        bool networkConnectFlag;
        bool m_hasReceiveData;

    protected:
        MainWindow(QWidget *parent);
        bool construct();

        bool initMenuBar();
        bool initMenuFile(QMenu *menu);
        bool initMenuCode(QMenu *menu);
        bool initMenuHelp(QMenu *menu);
        bool initWindow();
        bool initCentralWindet(QWidget *widget);
        bool initLeftWidget(QHBoxLayout *layout);
        bool initConnectWidget(QVBoxLayout *layout);
        bool initSetHostWidget(QVBoxLayout *layout);
        bool initPortWidget(QVBoxLayout *layout, QString text);
        bool initRightWidget(QHBoxLayout *layout);

        void handleConnect();
        void handleListen();
        void handleUdp();

        void onReadyRead(QTcpSocket *socket);

    protected slots:
        void onTriggeredSendFile();
        void onTriggeredReceiveFile();
        void onToggledAscii(bool flag);
        void onToggledUtf_8(bool flag);
        void onToggledTcpClient(bool flag);
        void onToggledTcpServer(bool flag);
        void onToggledUdp(bool flag);

        void onConnectButton();
        void onSendData();

        void onClientReadyRead();

        void onNewConnection();
        void onServerDisconnected();
        void onServerReadyRead();

        void clearSendBuffer();
        void clearReceiveBuffer();

    public:
        static MainWindow *NewInstance(QWidget *parent = NULL);
        ~MainWindow();
};

#endif // MAINWINDOW_H
