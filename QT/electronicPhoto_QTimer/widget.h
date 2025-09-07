#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
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

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void on_commit_Button_clicked();
    void on_stop_Button_clicked();

private:
    Ui::Widget *ui;
    QTimer* m_Timer;
    const uint16_t  TIME_OUT = 1000;            // 1S
    int m_current_index;                        // 图片索引
    QStringList m_image_list;                      // 存储图片文件名列表

    void timeEvent();

};
#endif // WIDGET_H
