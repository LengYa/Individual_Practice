#include "widget.h"
#include "./ui_widget.h"

#include <QMessageBox>
#include <QHostAddress>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("Tcp Client");

    socket = new QTcpSocket;

    //给新增的TCP槽函数，连接上信号
    connect(socket, &QTcpSocket::connected, this, &Widget::onSocketConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Widget::onSocketDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &Widget::onSocketReadRead);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
                   this, &Widget::onSocketError);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_connect_Button_clicked()
{
    //如果已连接，就断开连接
    if(socket->state() == QAbstractSocket::ConnectedState){
        socket->disconnectFromHost();
        ui->connect_Button->setText("连接网络");
        return;
    }

    //先获取IP地址和端口号
    QString server_ip = ui->server_address_lineEdit->text();
    QString server_port = ui->server_port_lineEdit->text();

    if(server_ip.isEmpty() || server_port.isEmpty()){
        QMessageBox::warning(this, "警告", "服务器地址或端口不能为空");
        return;
    }else{
        socket->connectToHost(QHostAddress(server_ip), server_port.toShort());

        //设置按钮为“正在连接...”
        ui->connect_Button->setText("正在连接...");
        ui->connect_Button->setEnabled(false);
    }
}

void Widget::on_send_Button_clicked()
{
    //如果服务器未连接，返回
    if(socket->state() != QAbstractSocket::ConnectedState){
        QMessageBox::warning(this, "警告", "未连接服务器");
        return;
    }

    QString message = ui->send_textEdit->toPlainText();
    if(message.isEmpty()){
        QMessageBox::warning(this, "警告","发送内容不能为空");
        return;
    }

    //开始发送数据
    QByteArray ba = message.toUtf8();
    socket->write(ba);

    // 在接收框显示自己发送的消息
    ui->receive_textBrowser->append("client: " + message);

    // 清空发送框
    ui->send_textEdit->clear();
}

void Widget::onSocketConnected()
{
    qDebug ()<<"连接服务器成功！";
    ui->connect_Button->setText("断开连接");
    ui->connect_Button->setEnabled(true);
}

void Widget::onSocketDisconnected()
{
    qDebug()<<"与服务器断开连接";
    ui->connect_Button->setText("连接网络");
    ui->connect_Button->setEnabled(true);
}

void Widget::onSocketReadRead()
{
    //接收数据
    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);

    ui->receive_textBrowser->append("server: "+ message);
}

void Widget::onSocketError(QAbstractSocket::SocketError error)
{
    qDebug()<<"连接错误:"<<socket->errorString();
    ui->connect_Button->setText("连接网络");
    ui->connect_Button->setEnabled(true);
    QMessageBox::warning(this, "错误","系统连接错误...");
}


void Widget::on_clear_receive_Button_clicked()
{
    ui->receive_textBrowser->clear();
}

