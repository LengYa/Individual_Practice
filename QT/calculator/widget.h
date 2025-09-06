#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>

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
    void on_number0_Button_clicked();

    void on_number1_Button_clicked();

    void on_number2_Button_clicked();

    void on_number3_Button_clicked();

    void on_number4_Button_clicked();

    void on_number5_Button_clicked();

    void on_numb6_Button_clicked();

    void on_number7_Button_clicked();

    void on_number8_Button_clicked();

    void on_number9_Button_clicked();

    void on_dev_Button_clicked();

    void on_multi_Button_clicked();

    void on_diff_Button_clicked();

    void on_plus_Button_clicked();

    void on_equval_Button_clicked();

    void on_dot_Button_clicked();

    void on_leftBracket_Button_clicked();

    void on_rightBracket_Button_clicked();

private:
    Ui::Widget *ui;
    QString express;

    QString infixToPostFix(const QString& str);
    double calPostFix(const QString& str);
};
#endif // WIDGET_H
