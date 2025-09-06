/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QGridLayout *gridLayout;
    QPushButton *number7_Button;
    QPushButton *number5_Button;
    QPushButton *number4_Button;
    QPushButton *clear_Button;
    QPushButton *number1_Button;
    QPushButton *dev_Button;
    QPushButton *numb6_Button;
    QPushButton *dot_Button;
    QPushButton *multi_Button;
    QPushButton *plus_Button;
    QPushButton *del_Button;
    QPushButton *number3_Button;
    QLineEdit *display_LineEdit;
    QPushButton *diff_Button;
    QPushButton *number2_Button;
    QPushButton *number9_Button;
    QPushButton *number8_Button;
    QPushButton *equval_Button;
    QPushButton *number0_Button;
    QPushButton *leftBracket_Button;
    QPushButton *rightBracket_Button;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(460, 468);
        gridLayout = new QGridLayout(Widget);
        gridLayout->setObjectName("gridLayout");
        number7_Button = new QPushButton(Widget);
        number7_Button->setObjectName("number7_Button");

        gridLayout->addWidget(number7_Button, 2, 0, 1, 1);

        number5_Button = new QPushButton(Widget);
        number5_Button->setObjectName("number5_Button");

        gridLayout->addWidget(number5_Button, 3, 1, 1, 1);

        number4_Button = new QPushButton(Widget);
        number4_Button->setObjectName("number4_Button");

        gridLayout->addWidget(number4_Button, 3, 0, 1, 1);

        clear_Button = new QPushButton(Widget);
        clear_Button->setObjectName("clear_Button");

        gridLayout->addWidget(clear_Button, 1, 0, 1, 1);

        number1_Button = new QPushButton(Widget);
        number1_Button->setObjectName("number1_Button");

        gridLayout->addWidget(number1_Button, 4, 0, 1, 1);

        dev_Button = new QPushButton(Widget);
        dev_Button->setObjectName("dev_Button");

        gridLayout->addWidget(dev_Button, 1, 3, 1, 1);

        numb6_Button = new QPushButton(Widget);
        numb6_Button->setObjectName("numb6_Button");

        gridLayout->addWidget(numb6_Button, 3, 2, 1, 1);

        dot_Button = new QPushButton(Widget);
        dot_Button->setObjectName("dot_Button");

        gridLayout->addWidget(dot_Button, 1, 1, 1, 1);

        multi_Button = new QPushButton(Widget);
        multi_Button->setObjectName("multi_Button");

        gridLayout->addWidget(multi_Button, 2, 3, 1, 1);

        plus_Button = new QPushButton(Widget);
        plus_Button->setObjectName("plus_Button");

        gridLayout->addWidget(plus_Button, 4, 3, 1, 1);

        del_Button = new QPushButton(Widget);
        del_Button->setObjectName("del_Button");

        gridLayout->addWidget(del_Button, 1, 2, 1, 1);

        number3_Button = new QPushButton(Widget);
        number3_Button->setObjectName("number3_Button");

        gridLayout->addWidget(number3_Button, 4, 2, 1, 1);

        display_LineEdit = new QLineEdit(Widget);
        display_LineEdit->setObjectName("display_LineEdit");

        gridLayout->addWidget(display_LineEdit, 0, 0, 1, 4);

        diff_Button = new QPushButton(Widget);
        diff_Button->setObjectName("diff_Button");

        gridLayout->addWidget(diff_Button, 3, 3, 1, 1);

        number2_Button = new QPushButton(Widget);
        number2_Button->setObjectName("number2_Button");

        gridLayout->addWidget(number2_Button, 4, 1, 1, 1);

        number9_Button = new QPushButton(Widget);
        number9_Button->setObjectName("number9_Button");

        gridLayout->addWidget(number9_Button, 2, 2, 1, 1);

        number8_Button = new QPushButton(Widget);
        number8_Button->setObjectName("number8_Button");

        gridLayout->addWidget(number8_Button, 2, 1, 1, 1);

        equval_Button = new QPushButton(Widget);
        equval_Button->setObjectName("equval_Button");
        equval_Button->setAutoFillBackground(false);
        equval_Button->setIconSize(QSize(40, 40));
        equval_Button->setAutoDefault(false);

        gridLayout->addWidget(equval_Button, 5, 3, 1, 1);

        number0_Button = new QPushButton(Widget);
        number0_Button->setObjectName("number0_Button");

        gridLayout->addWidget(number0_Button, 5, 1, 1, 1);

        leftBracket_Button = new QPushButton(Widget);
        leftBracket_Button->setObjectName("leftBracket_Button");

        gridLayout->addWidget(leftBracket_Button, 5, 0, 1, 1);

        rightBracket_Button = new QPushButton(Widget);
        rightBracket_Button->setObjectName("rightBracket_Button");

        gridLayout->addWidget(rightBracket_Button, 5, 2, 1, 1);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        number7_Button->setText(QCoreApplication::translate("Widget", "7", nullptr));
        number5_Button->setText(QCoreApplication::translate("Widget", "5", nullptr));
        number4_Button->setText(QCoreApplication::translate("Widget", "4", nullptr));
        clear_Button->setText(QCoreApplication::translate("Widget", "C", nullptr));
        number1_Button->setText(QCoreApplication::translate("Widget", "1", nullptr));
        dev_Button->setText(QCoreApplication::translate("Widget", "/", nullptr));
        numb6_Button->setText(QCoreApplication::translate("Widget", "6", nullptr));
        dot_Button->setText(QCoreApplication::translate("Widget", ".", nullptr));
        multi_Button->setText(QCoreApplication::translate("Widget", "X", nullptr));
        plus_Button->setText(QCoreApplication::translate("Widget", "+", nullptr));
        del_Button->setText(QCoreApplication::translate("Widget", "Del", nullptr));
        number3_Button->setText(QCoreApplication::translate("Widget", "3", nullptr));
        diff_Button->setText(QCoreApplication::translate("Widget", "-", nullptr));
        number2_Button->setText(QCoreApplication::translate("Widget", "2", nullptr));
        number9_Button->setText(QCoreApplication::translate("Widget", "9", nullptr));
        number8_Button->setText(QCoreApplication::translate("Widget", "8", nullptr));
        equval_Button->setText(QCoreApplication::translate("Widget", "=", nullptr));
        number0_Button->setText(QCoreApplication::translate("Widget", "0", nullptr));
        leftBracket_Button->setText(QCoreApplication::translate("Widget", "(", nullptr));
        rightBracket_Button->setText(QCoreApplication::translate("Widget", ")", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
