#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QStringList>

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
    void on_commit_Button_clicked();

    void on_stop_Button_clicked();

private:
    Ui::Widget *ui;
    const uint64_t TIME_OUT = 1000;                //1S
    int m_Timer_id;
    QStringList m_image_list;                      // 存储图片文件名列表
    int m_current_index;                           // 当前图片的索引

    virtual void timerEvent(QTimerEvent *event);
protected:
    void resizeEvent(QResizeEvent *event);
};
#endif // WIDGET_H
