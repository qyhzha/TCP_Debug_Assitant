#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    networkConnectFlag = false;
    m_hasReceiveData = false;
}

bool MainWindow::construct()
{
    assert(initMenuBar());
    assert(initWindow());

    return true;
}

bool MainWindow::initMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    if (menuBar == NULL) return false;

    QMenu *File = new QMenu("File", this);
    QMenu *Code = new QMenu("Code", this);
    QMenu *Help = new QMenu("Help", this);

    assert(File && Code && Help);

    assert(initMenuFile(File));
    assert(initMenuCode(Code));
    assert(initMenuHelp(Help));

    menuBar->addMenu(File);
    menuBar->addMenu(Code);
    menuBar->addMenu(Help);

    return true;
}

bool MainWindow::initMenuFile(QMenu *menu)
{
    assert(menu);

    sendFile = new QAction("Send File...", this);
    m_receiveToFile = new QAction("Receive File...", this);
    QAction *exit = new QAction("Exit...", this);

    assert(sendFile && m_receiveToFile && exit);

    sendFile->setEnabled(false);
    m_receiveToFile->setEnabled(false);

    connect(sendFile, SIGNAL(triggered()), this, SLOT(onTriggeredSendFile()));
    connect(m_receiveToFile, SIGNAL(triggered()), this, SLOT(onTriggeredReceiveFile()));
    connect(exit, SIGNAL(triggered()), this, SLOT(close()));

    menu->addAction(sendFile);
    menu->addAction(m_receiveToFile);
    menu->addAction(exit);

    return true;
}

bool MainWindow::initMenuCode(QMenu *menu)
{
    assert(menu);

    ascii = new QAction("ASCII...", this);
    utf_8 = new QAction("UTF-8...");

    assert(ascii && utf_8);

    ascii->setCheckable(true);
    utf_8->setCheckable(true);

    ascii->setChecked(false);
    utf_8->setChecked(true);

    connect(ascii, SIGNAL(toggled(bool)), this, SLOT(onToggledAscii(bool)));
    connect(utf_8, SIGNAL(toggled(bool)), this, SLOT(onToggledUtf_8(bool)));

    menu->addAction(ascii);
    menu->addAction(utf_8);

    return true;
}

bool MainWindow::initMenuHelp(QMenu *menu)
{
    if (menu == NULL)
    {
        qDebug() << __FUNCTION__ << ": parameter menu is null...";
        return false;
    }

    QAction *about = new QAction("about...", this);

    if (about == NULL)
    {
        qDebug() << __FUNCTION__ << ": No enough memory to create action object...";
        return false;
    }

    menu->addAction(about);

    return true;
}

bool MainWindow::initWindow()
{
    setWindowTitle(QString::fromUtf8("Tcp调试助手"));
    setWindowIcon(QIcon(":/new/main.jpg"));
    //resize(1000, 500);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QWidget *widget = new QWidget(this);
    assert(widget);

    assert(initCentralWindet(widget));

    setCentralWidget(widget);

    return true;
}

bool MainWindow::initCentralWindet(QWidget *widget)
{
    assert(widget);

    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    assert(hBoxLayout);

    hBoxLayout->setSpacing(4);

    assert(initLeftWidget(hBoxLayout));
    assert(initRightWidget(hBoxLayout));

    hBoxLayout->setContentsMargins(10, 20, 10, 20);

    widget->setLayout(hBoxLayout);

    return true;
}

bool MainWindow::initLeftWidget(QHBoxLayout *layout)
{
    assert(layout);

    m_light = new QLightLabel(this);
    connectButton = new QPushButton(this);
    QPushButton *clearSendBuffer = new QPushButton(this);
    QPushButton *clearm_receiveBuffer = new QPushButton(this);
    QPushButton *send = new QPushButton(this);
    QPushButton *exit = new QPushButton(this);
    assert(connectButton && clearSendBuffer && clearm_receiveBuffer && send && exit);

    m_light->setState(QLightLabel::Normal);
    m_light->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_light->setMinimumSize(20, 20);

    connectButton->setText("连接网络");
    connectButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connectButton->setMinimumWidth(140);
    connect(connectButton, SIGNAL(clicked()), this, SLOT(onConnectButton()));

    QHBoxLayout *h = new QHBoxLayout();

    h->setSpacing(10);
    h->setContentsMargins(4, 0, 4, 0);

    h->addWidget(m_light, 1, Qt::AlignCenter);
    h->addWidget(connectButton, 5, Qt::AlignCenter);

    clearSendBuffer->setText("清空发送区");
    clearSendBuffer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    clearSendBuffer->setMinimumWidth(200);
    connect(clearSendBuffer, SIGNAL(clicked()), this, SLOT(clearSendBuffer()));

    clearm_receiveBuffer->setText("清空接收区");
    clearm_receiveBuffer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    clearm_receiveBuffer->setMinimumWidth(200);
    connect(clearm_receiveBuffer, SIGNAL(clicked()), this, SLOT(clearReceiveBuffer()));

    send->setText("发送数据");
    send->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    send->setMinimumWidth(200);
    connect(send, SIGNAL(clicked()), this, SLOT(onSendData()));

    exit->setText("退出");
    exit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    exit->setMinimumWidth(200);
    connect(exit, SIGNAL(clicked()), this, SLOT(close()));

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    assert(vBoxLayout);

    vBoxLayout->setSpacing(10);
    vBoxLayout->setContentsMargins(10, 10, 10, 0);

    assert(initConnectWidget(vBoxLayout));
    assert(initSetHostWidget(vBoxLayout));
    vBoxLayout->addLayout(h, 2);
    vBoxLayout->addWidget(clearSendBuffer, 2, Qt::AlignCenter);
    vBoxLayout->addWidget(clearm_receiveBuffer, 2, Qt::AlignCenter);
    vBoxLayout->addWidget(send, 2, Qt::AlignCenter);
    vBoxLayout->addWidget(exit, 2, Qt::AlignCenter);

    layout->addLayout(vBoxLayout, 1);

    return true;
}

bool MainWindow::initConnectWidget(QVBoxLayout *layout)
{
    assert(layout);

    QGroupBox *connectBox = new QGroupBox(this);
    tcpClient = new QRadioButton(connectBox);
    tcpServer = new QRadioButton(connectBox);
    udp = new QRadioButton(connectBox);
    assert(connectBox && tcpClient && tcpServer && udp);

    connectBox->setTitle("连接方式:");
    connectBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connectBox->setMinimumWidth(200);

    tcpClient->setText("TCP 客户端");
    tcpClient->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tcpClient->setMinimumWidth(180);
    tcpClient->setChecked(true);

    tcpServer->setText("TCP 服务器");
    tcpServer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tcpServer->setMinimumWidth(180);
    tcpServer->setChecked(false);

    udp->setText("UDP");
    udp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    udp->setMinimumWidth(180);
    udp->setChecked(false);

    QVBoxLayout *vl = new QVBoxLayout();
    assert(vl);

    vl->setSpacing(4);
    vl->setContentsMargins(10, 10, 10, 10);
    vl->addWidget(tcpClient, 1, Qt::AlignLeft);
    vl->addWidget(tcpServer, 1, Qt::AlignLeft);
    vl->addWidget(udp, 1, Qt::AlignLeft);

    connectBox->setLayout(vl);

    layout->addWidget(connectBox, 6, Qt::AlignCenter);

    return true;
}

bool MainWindow::initSetHostWidget(QVBoxLayout *layout)
{
    assert(layout);

    QGroupBox *setBox = new QGroupBox(this);
    QLabel *hostlabel = new QLabel(this);
    hostIp = new QLineEdit(this);
    assert(setBox && hostlabel && hostIp);

    setBox->setTitle("设置:");
    setBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setBox->setMinimumWidth(200);
    hostlabel->setText("IP 地址");
    hostlabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    hostlabel->setMinimumWidth(180);
    hostIp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    hostIp->setMinimumWidth(180);
    hostIp->setMinimumHeight(30);

    QHBoxLayout *h1 = new QHBoxLayout();
    assert(h1);

    QLabel *distantLabel = new QLabel(this);
    hostPort = new QSpinBox(this);
    assert(distantLabel && hostPort);

    distantLabel->setText("端口： ");
    distantLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    hostPort->setRange(0, 65535);
    hostPort->setValue(80);
    hostPort->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    h1->addWidget(distantLabel, 1);
    h1->addWidget(hostPort, 1);

    h1->setSpacing(4);

    QVBoxLayout *v = new QVBoxLayout();
    assert(v);

    v->setSpacing(4);
    v->setContentsMargins(10, 10, 10, 10);
    v->addWidget(hostlabel, 1, Qt::AlignLeft);
    v->addWidget(hostIp, 1, Qt::AlignLeft);
    v->addLayout(h1, 1);

    setBox->setLayout(v);

    layout->addWidget(setBox, 9, Qt::AlignCenter);

    return true;
}

bool MainWindow::initPortWidget(QVBoxLayout *layout, QString text)
{
    assert(layout);

    QHBoxLayout *h = new QHBoxLayout();
    assert(h);

    QLabel *label = new QLabel(this);
    QSpinBox *intBox = new QSpinBox(this);
    assert(label && intBox);

    label->setText(text);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    intBox->setRange(0, 65535);
    intBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    h->addWidget(label, 1);
    h->addWidget(intBox, 1);

    h->setSpacing(4);
    layout->addLayout(h, 1);

    return true;
}

bool MainWindow::initRightWidget(QHBoxLayout *layout)
{
    assert(layout);

    QVBoxLayout *v = new QVBoxLayout();
    assert(v);

    QLabel *receiveLabel = new QLabel(this);
    m_receiveBuffer = new QPlainTextEdit(this);
    QLabel *sendLabel = new QLabel(this);
    m_sendBuffer = new QPlainTextEdit(this);

    assert(m_receiveBuffer && receiveLabel && m_sendBuffer && sendLabel);

    receiveLabel->setText("数据接收区");
    receiveLabel->setAlignment(Qt::AlignBottom);
    receiveLabel->setMinimumWidth(400);
    receiveLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_receiveBuffer->setMinimumWidth(400);
    m_receiveBuffer->setReadOnly(true);
    m_receiveBuffer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    sendLabel->setText("数据发送区");
    sendLabel->setAlignment(Qt::AlignBottom);
    sendLabel->setMinimumWidth(400);
    sendLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_sendBuffer->setMinimumWidth(400);
    m_sendBuffer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //v->setParent(layout);
    v->setSpacing(4);
    v->setContentsMargins(0, 5, 5, 5);

    v->addWidget(receiveLabel, 1, Qt::AlignCenter);
    v->addWidget(m_receiveBuffer, 15, Qt::AlignTop);
    v->addWidget(sendLabel, 1, Qt::AlignCenter);
    v->addWidget(m_sendBuffer, 8, Qt::AlignTop);

    layout->addLayout(v, 3);

    return true;
}

MainWindow *MainWindow::NewInstance(QWidget *parent)
{
    MainWindow *ret = new MainWindow(parent);

    if ((ret == NULL) || (!ret->construct()))
    {
        delete ret;
        ret = NULL;
    }

    return ret;
}

MainWindow::~MainWindow()
{

}
