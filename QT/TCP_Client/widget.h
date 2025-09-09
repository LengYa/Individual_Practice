#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_connect_Button_clicked();
    void on_send_Button_clicked();

    //添加几个TCP相关槽函数，连接成功，断开连接，连接失败，接收数据
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadRead();
    void onSocketError(QAbstractSocket::SocketError error);

    void on_clear_receive_Button_clicked();

private:
    Ui::Widget *ui;
    QTcpSocket* socket;
};
#endif // WIDGET_H
