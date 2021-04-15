#include "MainWindow.h"

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
            QMessageBox::critical(this, "出错了", QString("不能打开该文件！\n请检查是否有权限写入该文件...\n文件名：") + filename);
            return;
        }

        qDebug() << __FUNCTION__ << ": length = " << m_receiveBuffer->toPlainText().toStdString().length();
        qint64 bytes = file.write(m_receiveBuffer->toPlainText().toStdString().c_str(), m_receiveBuffer->toPlainText().toStdString().length());
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

void MainWindow::onConnectButton()
{
    static bool flag = false;
    if (tcpClient->isChecked())
    {
        if (!flag)
        {
            if (hostIp->text().isEmpty())
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

            m_socket->connectToHost(hostIp->text(), hostPort->value());
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
            m_light->setState(QLightLabel::Working);
            flag = true;
        }
        else
        {
            m_socket->close();

            qDebug() << __FUNCTION__ << ": Close network connect success!";

            sendFile->setEnabled(false);
            connectButton->setText("连接网络");
            m_light->setState(QLightLabel::Normal);
            flag = false;
        }
    }
    else
    {
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
        QMessageBox::critical(this, "出错了", "发送缓冲区没有数据...\n请在发送缓冲区写入待发送数据。\n");
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
