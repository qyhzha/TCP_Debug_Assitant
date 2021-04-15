#include "MainWindow.h"

void MainWindow::handleConnect()
{
    static bool connectFlag = false;

    if (!connectFlag)
    {
        if (m_hostIp->text().isEmpty())
        {
            QMessageBox::critical(this, "出错了", "IP地址不能为空");
            return;
        }

        m_socket = new QTcpSocket(this);
        if (m_socket == NULL)
        {
            QMessageBox::critical(this, "出错了", "创建TCP连接失败！\n请稍后再试...");
            qDebug() << "Create tcp socket error...";
            return;
        }

        m_socket->connectToHost(m_hostIp->text(), hostPort->value());
        if (m_socket->waitForConnected(3000) == false)
        {
            QMessageBox::critical(this, "出错了", "连接目标地址失败！\n请检查目标地址是否是合法的域名地址...");
            qDebug() << "Connect error...";
            return;
        }

        qDebug() << __FUNCTION__ << ": connect success!";

        connect(m_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

        sendFile->setEnabled(true);
        connectButton->setText("断开连接");
        m_light->setState(true);
        connectFlag = true;
    }
    else
    {
        m_socket->close();

        qDebug() << __FUNCTION__ << ": Close network connect success!";

        sendFile->setEnabled(false);
        connectButton->setText("连接网络");
        m_light->setState(false);
        connectFlag = false;
    }
}

void MainWindow::handleListen()
{

}

void MainWindow::handleUdp()
{

}

void MainWindow::onTriggeredSendFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "发送文件：");

    if (filename.isEmpty() == false)
    {
        qDebug() << __FUNCTION__ << "： filename = " << filename;

        QFile file(filename);

        if (file.open(QIODevice::ReadOnly) == false)
        {
            QMessageBox::critical(this, "出错了",
                                  QString("不能打开该文件！\n请检查是否有权限读取该文件...\n文件名：")
                                  + filename);
            return;
        }

        QByteArray data = file.readAll();
        qDebug() << __FUNCTION__ << "： data = " << data;

        qint64 bytes = m_socket->write(data);
        qDebug() << __FUNCTION__ << ": bytes = " << bytes;

        file.close();
    }
}

void MainWindow::onTriggeredReceiveFile()
{
    QString filename = QFileDialog::getSaveFileName(this, "保存到文件：");

    if (filename.isEmpty() == false)
    {
        qDebug() << __FUNCTION__ << "： filename = " << filename;

        QFile file(filename);

        if (file.open(QIODevice::WriteOnly) == false)
        {
            QMessageBox::critical(this, "出错了",
                                  QString("不能打开该文件！\n请检查是否有权限写入该文件...\n文件名：")
                                  + filename);
            return;
        }

        qDebug() << __FUNCTION__ << ": length = " <<
                 m_receiveBuffer->toPlainText().toStdString().length();
        qint64 bytes = file.write(m_receiveBuffer->toPlainText().toStdString().c_str(),
                                  m_receiveBuffer->toPlainText().toStdString().length());
        qDebug() << __FUNCTION__ << ": bytes = " << bytes;
        file.close();
    }
}

void MainWindow::onToggledAscii(bool flag)
{
    utf_8->setChecked(!flag);
}

void MainWindow::onToggledUtf_8(bool flag)
{
    ascii->setChecked(!flag);
}

void MainWindow::onToggledTcpClient(bool flag)
{
    if (flag)
    {
        m_hostIp->setText(m_tcpClientRecord.hostIpStr);
        m_hostIp->setReadOnly(false);
        m_light->setState(m_tcpClientRecord.lightState);
        connectButton->setText("连接网络");
    }
    else
    {
        m_tcpClientRecord.hostIpStr = m_hostIp->text();
        m_tcpClientRecord.lightState = m_light->state();
    }
}

void MainWindow::onToggledTcpServer(bool flag)
{
    if (flag)
    {
        m_hostIp->clear();
        m_hostIp->setReadOnly(true);
        connectButton->setText("开始监听");
    }
}

void MainWindow::onToggledUdp(bool flag)
{
    if (flag)
    {
        m_hostIp->clear();
        m_hostIp->setReadOnly(false);
        connectButton->setText("开启UDP");
    }
}

void MainWindow::onConnectButton()
{
    if (m_tcpClient->isChecked())
    {
        handleConnect();
    }
    else if (m_tcpServer->isChecked())
    {
        handleListen();
    }
    else if (m_udp->isChecked())
    {
        handleUdp();
    }
    else
    {
        QMessageBox::critical(this, "出错了", "未知的操作...");
        qDebug() << "don't know error...";
    }
}

void MainWindow::onReadyRead()
{
    m_receiveToFile->setEnabled(true);
    m_hasReceiveData = true;

    QByteArray data = m_socket->readAll();

    qDebug() << __FUNCTION__ << ": bytes = " << data.length();

    QString text = m_receiveBuffer->toPlainText();

    if (text.length() >= 20000)
    {
        text.clear();
        text = "";
    }

    m_receiveBuffer->setPlainText(text + data);
}

void MainWindow::onSendData()
{
    QString data = m_sendBuffer->toPlainText();

    if (data.isEmpty())
    {
        QMessageBox::critical(this, "出错了",
                              "发送缓冲区没有数据...\n请在发送缓冲区写入待发送数据。\n");
        return;
    }
    else
    {
        qint64 bytes = m_socket->write(data.toStdString().c_str(), data.length());
        qDebug() << __FUNCTION__ << ": bytes = " << bytes;
    }
}

void MainWindow::clearSendBuffer()
{
    m_sendBuffer->clear();
    m_sendBuffer->setPlainText("");
}

void MainWindow::clearReceiveBuffer()
{
    if (m_hasReceiveData)
    {
        m_receiveBuffer->clear();
        m_receiveBuffer->setPlainText("");
        m_hasReceiveData = false;
        m_receiveToFile->setEnabled(false);
    }
}
