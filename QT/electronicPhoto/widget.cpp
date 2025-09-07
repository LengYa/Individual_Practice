#include "widget.h"
#include "./ui_widget.h"

#include <QDir>
#include <QDebug>
#include <QPixmap>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("ElectronicPhoto");
    m_current_index = 0;

    qDebug()<<"current path:"<<QDir::currentPath();         //获取当前路径，便于调整图片相对路径
    //图片路径
    QString path = "../../images";

    //获取文件下所有的图片
    QDir dir(path);

    if(!dir.exists()){
        qDebug() << "Directory does not exist:" << path;
        return;
    }

    if(!dir.isReadable()){
        qDebug()<<"Directory does not read:"<<path;
        return;
    }

    QStringList filter;
    filter<<"*.jpg"<<"*.png"<<".bmp";
    QStringList image_list;
    image_list = dir.entryList(filter, QDir::Files);
    for(int i = 0; i < image_list.length(); ++i){
        QString str = path + '/' + image_list[i];
        m_image_list.append(str);
    }

    // 显示第一张图片（如果有）
    if (!m_image_list.isEmpty()) {
        QString firstImage = m_image_list.first();
        ui->photo_label->setPixmap(QPixmap(firstImage).scaled(ui->photo_label->size(),
                                                              Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_commit_Button_clicked()
{
    //启动定时器
    m_Timer_id = this->startTimer(TIME_OUT);
}


void Widget::on_stop_Button_clicked()
{
    this->killTimer(m_Timer_id);
}

void Widget::timerEvent(QTimerEvent *event)
{

    if(event->timerId() != m_Timer_id)  return;
    if(m_image_list.isEmpty())  return;

    //更新索引
    m_current_index = (m_current_index + 1) % m_image_list.size();

    QString image_path = m_image_list[m_current_index];
    QPixmap pixmap(image_path);

    if(!pixmap.isNull()){
        ui->photo_label->setPixmap(pixmap.scaled(ui->photo_label->size(),
                                                 Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);                //调用基类实现

    if(!m_image_list.isEmpty() && m_current_index >= 0){
        QString image_path = m_image_list[m_current_index];
        QPixmap pixmap(image_path);
        if(!pixmap.isNull()){
            ui->photo_label->setPixmap(pixmap.scaled(ui->photo_label->size(),
                                                     Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}
