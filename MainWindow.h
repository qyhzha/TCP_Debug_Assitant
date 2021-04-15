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
#include <QDebug>

#include "QLightLabel.h"

class MainWindow : public QMainWindow
{
        Q_OBJECT

    protected:
        struct tcpClientRecord : public QObject
        {
            QString hostIpStr;
            bool lightState;
        };

    protected:
        QAction *sendFile;
        QAction *m_receiveToFile;
        QAction *ascii;
        QAction *utf_8;
        QRadioButton *m_tcpClient;
        tcpClientRecord m_tcpClientRecord;
        QRadioButton *m_tcpServer;
        QRadioButton *m_udp;
        QLineEdit *m_hostIp;
        QSpinBox *hostPort;
        QLightLabel *m_light;
        QPushButton *connectButton;
        QPlainTextEdit *m_receiveBuffer;
        QPlainTextEdit *m_sendBuffer;

        bool networkConnectFlag;
        bool m_hasReceiveData;

        QTcpSocket *m_socket;

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

    protected slots:
        void onTriggeredSendFile();
        void onTriggeredReceiveFile();
        void onToggledAscii(bool flag);
        void onToggledUtf_8(bool flag);
        void onToggledTcpClient(bool flag);
        void onToggledTcpServer(bool flag);
        void onToggledUdp(bool flag);
        void onConnectButton();
        void onReadyRead();
        void onSendData();
        void clearSendBuffer();
        void clearReceiveBuffer();

    public:
        static MainWindow *NewInstance(QWidget *parent = NULL);
        ~MainWindow();
};

#endif // MAINWINDOW_H
