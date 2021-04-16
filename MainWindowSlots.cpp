#include "MainWindow.h"

void MainWindow::handleConnect()
{
    if (m_button->text() == "连接网络")
    {
        if (m_hostIp->text().isEmpty())
        {
            QMessageBox::critical(this, "出错了", "IP地址不能为空");
            return;
        }

        m_tcpClient->connectToHost(m_hostIp->text(), hostPort->value());
        if (m_tcpClient->waitForConnected(3000) == false)
        {
            QMessageBox::critical(this, "出错了", "连接目标地址失败！\n请检查目标地址是否是合法的域名地址...");
            qDebug() << "Connect error...";
            return;
        }

        connect(m_tcpClient, SIGNAL(readyRead()), this, SLOT(onClientReadyRead()));
        connect(m_tcpClient, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));

        m_tcpClientRecord.hostIpStr = m_hostIp->text();
        m_tcpClientRecord.buttonStr = "断开连接";
        m_tcpClientRecord.sendFileEnable = true;
        m_tcpClientRecord.lightState = true;

        updateRecord();

        qDebug() << __FUNCTION__ << ": connect success!";
    }
    else if (m_button->text() == "断开连接")
    {
        m_tcpClient->close();

        m_tcpClientRecord.hostIpStr = m_hostIp->text();
        m_tcpClientRecord.buttonStr = "连接网络";
        m_tcpClientRecord.sendFileEnable = false;
        m_tcpClientRecord.lightState = false;

        updateRecord();

        qDebug() << __FUNCTION__ << ": Close network connect success!";
    }
}

void MainWindow::handleListen()
{
    if (m_button->text() == "开启监听")
    {
        if (m_tcpServer->listen(QHostAddress::Any, hostPort->value()) != true)
        {
            QMessageBox::critical(this, "出错了", "监听目标端口失败！\n请检查目标端口是否被占用...");
            qDebug() << "Listen error...";
            return;
        }

        connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

        m_tcpServerRecord.buttonStr = "结束监听";
        m_tcpServerRecord.lightState = false;
        m_tcpServerRecord.sendFileEnable = false;

        updateRecord();

        qDebug() << __FUNCTION__ << ": listern success!";
    }
    else if (m_button->text() == "结束监听")
    {
        QList<QTcpSocket *> tcpClientList = m_tcpServer->findChildren<QTcpSocket *>();
        if (tcpClientList[tcpClientList.length() - 1] != 0)
        {
            tcpClientList[tcpClientList.length() - 1]->close();
        }

        m_tcpServer->close();

        m_tcpServerRecord.buttonStr = "开启监听";
        m_tcpServerRecord.sendFileEnable = false;
        m_tcpServerRecord.lightState = false;

        updateRecord();

        qDebug() << __FUNCTION__ << ": Close listen success!";
    }
}

void MainWindow::handleUdp()
{

}

void MainWindow::updateRecord()
{
    if (m_tcpClientButton->isChecked())
    {
        m_hostIp->setText(m_tcpClientRecord.hostIpStr);
        m_hostIp->setReadOnly(false);
        m_button->setText(m_tcpClientRecord.buttonStr);
        m_light->setState(m_tcpClientRecord.lightState);
        m_sendFileAction->setEnabled(m_tcpClientRecord.sendFileEnable);
    }
    else if (m_tcpServerButton->isChecked())
    {
        m_hostIp->clear();
        m_hostIp->setReadOnly(true);
        m_button->setText(m_tcpServerRecord.buttonStr);
        m_light->setState(m_tcpServerRecord.lightState);
        m_sendFileAction->setEnabled(m_tcpServerRecord.sendFileEnable);
    }
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
            QMessageBox::critical(this, "出错了", QString("不能打开该文件！\n请检查是否有权限读取该文件...\n文件名：") + filename);
            return;
        }

        QByteArray data = file.readAll();
        qDebug() << __FUNCTION__ << "： data = " << data;

        qint64 bytes = m_tcpClient->write(data);
        qDebug() << __FUNCTION__ << ": bytes = " << bytes;

        file.close();
    }
}

void MainWindow::onTriggeredReceiveFile()
{
    QString filename = QFileDialog::getSaveFileName(this, "保存到文件：");
    if (filename.isEmpty())
    {
        return;
    }

    QFile file(filename);

    if (file.open(QIODevice::WriteOnly) == false)
    {
        QMessageBox::critical(this, "出错了", QString("不能打开该文件！\n请检查是否有权限写入该文件...\n文件名：") + filename);
        return;
    }

    file.write(m_receiveBuffer->toPlainText().toStdString().c_str(), m_receiveBuffer->toPlainText().toStdString().length());

    file.close();

    clearReceiveBuffer();

    qDebug() << __FUNCTION__ << ": length = " << m_receiveBuffer->toPlainText().toStdString().length();
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
        updateRecord();
        clearReceiveBuffer();
    }
    else
    {
        m_tcpClientRecord.hostIpStr = m_hostIp->text();
        m_tcpClientRecord.buttonStr = m_button->text();
        m_tcpClientRecord.lightState = m_light->state();
        m_tcpClientRecord.sendFileEnable = m_sendFileAction->isEnabled();
    }
}

void MainWindow::onToggledTcpServer(bool flag)
{
    if (flag)
    {
        updateRecord();
        clearReceiveBuffer();
    }
    else
    {
        m_tcpServerRecord.buttonStr = m_button->text();
        m_tcpServerRecord.lightState = m_light->state();
        m_tcpServerRecord.sendFileEnable = m_sendFileAction->isEnabled();
    }
}

void MainWindow::onToggledUdp(bool flag)
{
    if (flag)
    {
        m_hostIp->clear();
        m_hostIp->setReadOnly(false);
        m_button->setText("开启UDP");
    }
}

void MainWindow::onConnectButton()
{
    if (m_tcpClientButton->isChecked())
    {
        handleConnect();
    }
    else if (m_tcpServerButton->isChecked())
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

void MainWindow::onSendData()
{
    QString data = m_sendBuffer->toPlainText();

    if (data.isEmpty())
    {
        QMessageBox::critical(this, "出错了", "发送缓冲区没有数据...\n请在发送缓冲区写入待发送数据。\n");
        return;
    }

    if (m_tcpClientButton->isChecked())
    {
        qint64 bytes = m_tcpClient->write(data.toStdString().c_str(), data.length());
        qDebug() << __FUNCTION__ << ": bytes = " << bytes;
    }
    else if (m_tcpServerButton->isChecked())
    {
        QList<QTcpSocket *> tcpClientList = m_tcpServer->findChildren<QTcpSocket *>();
        if (tcpClientList[tcpClientList.length() - 1] != 0)
        {
            tcpClientList[tcpClientList.length() - 1]->write(data.toStdString().c_str(), data.length());
        }
        else
        {
            QMessageBox::critical(this, "出错了", "没有已经接入的连接...");
        }
    }
}

void MainWindow::onReadyRead(QTcpSocket *socket)
{
    if (socket == NULL) return;

    m_receiveToFile->setEnabled(true);
    m_hasReceiveData = true;

    QByteArray data = socket->readAll();

    qDebug() << __FUNCTION__ << ": bytes = " << data.length();

    QString text = m_receiveBuffer->toPlainText();

    if (text.length() >= 20480)
    {
        text.clear();
        text = "";
    }

    m_receiveBuffer->setPlainText(text + data);
}

void MainWindow::onClientReadyRead()
{
    onReadyRead(m_tcpClient);
}

void MainWindow::onClientDisconnected()
{
    qDebug() << __FUNCTION__ << ": Close network connect success!";

    //m_tcpClientRecord.hostIpStr = m_hostIp->text();
    m_tcpClientRecord.buttonStr = "连接网络";
    m_tcpClientRecord.lightState = false;
    m_tcpClientRecord.sendFileEnable = false;

    updateRecord();
}

void MainWindow::onNewConnection()
{
    QTcpSocket* tcpClient = m_tcpServer->nextPendingConnection();

    QMessageBox::information(this, "提示",
                             tcpClient->peerAddress().toString() + ":" + QString::number(tcpClient->peerPort()) + "已连接",
                             QMessageBox::Ok);

    connect(tcpClient, SIGNAL(disconnected()), this, SLOT(onServerDisconnected()));
    connect(tcpClient, SIGNAL(readyRead()), this, SLOT(onServerReadyRead()));

    m_tcpServer->pauseAccepting();

    m_tcpServerRecord.buttonStr = "结束监听";
    m_tcpServerRecord.sendFileEnable = true;
    m_tcpServerRecord.lightState = true;

    updateRecord();
}

void MainWindow::onServerDisconnected()
{
    QTcpSocket* tcpClient = dynamic_cast<QTcpSocket*>(sender());
    if (tcpClient == NULL) return;

    tcpClient->deleteLater();

    QMessageBox::information(this, "提示",
                             tcpClient->peerAddress().toString() + ":" + QString::number(tcpClient->peerPort()) + "已断开连接",
                             QMessageBox::Ok);

    m_tcpServer->resumeAccepting();

    m_tcpServerRecord.buttonStr = "结束监听";
    m_tcpServerRecord.sendFileEnable = false;
    m_tcpServerRecord.lightState = false;

    updateRecord();
}

void MainWindow::onServerReadyRead()
{
    QTcpSocket* tcpClient = dynamic_cast<QTcpSocket*>(sender());
    if (tcpClient == NULL) return;

    onReadyRead(tcpClient);
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
