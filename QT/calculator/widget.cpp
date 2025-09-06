#include <QStack>
#include <QChar>

#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    express.clear();

    //连接信号与槽，对C按钮使用
    connect(ui->clear_Button, &QPushButton::clicked, this, [this](){
        express.clear();
        ui->display_LineEdit->setText(express);
    });

    //连接信号与槽，对Del按钮使用
    connect(ui->del_Button, &QPushButton::clicked, this, [this](){
        if(!express.isEmpty()){
            express.chop(1);
            ui->display_LineEdit->setText(express);
        }
    });

    //设置窗口样式
    this->setMaximumSize(300,380);
    this->setMinimumSize(300, 380);
    this->setWindowTitle("计算器");

    ui->equval_Button->setStyleSheet("background-color: green;");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_number0_Button_clicked()
{
    express += '0';
    ui->display_LineEdit->setText(express);
}


void Widget::on_number1_Button_clicked()
{
    express += '1';
    ui->display_LineEdit->setText(express);
}


void Widget::on_number2_Button_clicked()
{
    express += '2';
    ui->display_LineEdit->setText(express);
}

void Widget::on_number3_Button_clicked()
{
    express += '3';
    ui->display_LineEdit->setText(express);
}

void Widget::on_number4_Button_clicked()
{
    express += '4';
    ui->display_LineEdit->setText(express);
}

void Widget::on_number5_Button_clicked()
{
    express += '5';
    ui->display_LineEdit->setText(express);
}


void Widget::on_numb6_Button_clicked()
{
    express += '6';
    ui->display_LineEdit->setText(express);
}


void Widget::on_number7_Button_clicked()
{
    express += '7';
    ui->display_LineEdit->setText(express);
}


void Widget::on_number8_Button_clicked()
{
    express += '8';
    ui->display_LineEdit->setText(express);
}


void Widget::on_number9_Button_clicked()
{
    express += '9';
    ui->display_LineEdit->setText(express);
}


void Widget::on_dev_Button_clicked()
{
    express += '/';
    ui->display_LineEdit->setText(express);
}


void Widget::on_multi_Button_clicked()
{
    express += 'X';
    ui->display_LineEdit->setText(express);
}


void Widget::on_diff_Button_clicked()
{
    express += '-';
    ui->display_LineEdit->setText(express);
}


void Widget::on_plus_Button_clicked()
{
    express += '+';
    ui->display_LineEdit->setText(express);
}

void Widget::on_dot_Button_clicked()
{
    express += '.';
    ui->display_LineEdit->setText(express);
}


void Widget::on_leftBracket_Button_clicked()
{
    express += '(';
    ui->display_LineEdit->setText(express);
}

void Widget::on_rightBracket_Button_clicked()
{
    express += ')';
    ui->display_LineEdit->setText(express);
}

QString Widget::infixToPostFix(const QString &str)
{
    QStack<QChar> ops;
    QString ans = "";

    auto priority = [](const QChar c)->int{
        if(c == '+' || c== '-') return 1;
        if(c == '*' || c== '/') return 2;
        return 0;
    };

    int len = str.length();
    for(int i = 0; i < len; ++i){
        QChar c= str[i];
        //处理数字和小数点
        if(c == '.' || c.isDigit()){
            while(i < len && (str[i] == '.' || str[i].isDigit())){
                ans += str[i];
                ++i;
            }
            i--;        //再回退一格
            ans += ' ';     //用空格分隔数字
        }else if(c == '('){
            ops.push(c);
        }else if(c == ')'){
            while(!ops.isEmpty() && ops.top() != '('){
                ans += ops.top();
                ops.pop();
                ans += ' ';
            }
            if(!ops.isEmpty() && ops.top() == '('){
                ops.pop();
            }
        }else{          //处理四则运算符
            while(!ops.isEmpty() && (priority(ops.top()) > priority(c))){
                ans += ops.top();
                ops.pop();
                ans += ' ';
            }
            ops.push(c);
        }
    }

    //将剩下的运算符全部弹出
    while(!ops.isEmpty()){
        ans += ops.top();
        ops.pop();
        ans += ' ';
    }

    return ans.trimmed();
}

double Widget::calPostFix(const QString &str)
{
    QStack<double> nums;
    QString num_str;
    int len = str.length();

    auto isOperator = [](const QChar& c)->bool{
        if(c == '+' || c == '-' || c == '*' || c == '/')    return true;
        return false;
    };

    for(int i = 0; i < len; ++i){
        QChar c = str[i];

        //先处理数字和小数点
        if(c == '.' || c.isDigit()){
            num_str.clear();
            while(i < len && (str[i].isDigit() || str[i] == '.')){
                num_str += str[i];
                ++i;
            }
            i--;

            bool flag = false;
            double num = num_str.toDouble(&flag);
            if(flag){
                nums.push(num);
            }
        }else if(isOperator(c)){
            //操作如果<2不允许计算
            if(nums.size() < 2){
                return 0;
            }

            double num_B = nums.top();
            nums.pop();
            double num_A = nums.top();
            nums.pop();

            double result = 0.0f;
            switch(c.unicode()){
            case '+': result = num_A + num_B; break;
            case '-': result = num_A - num_B; break;
            case '*': result = num_A * num_B; break;
            case '/': {
                if(num_B == 0){
                    return 0;
                }
                result = num_A / num_B;
            }
            break;
            }
            nums.push(result);
        }
    }
    if(nums.isEmpty()){
        return 0;
    }
    return nums.top();
}


void Widget::on_equval_Button_clicked()
{
    if(express.isEmpty()){
        express += '0';
        ui->display_LineEdit->setText(express);
        return;
    }
    //逆波兰式进行计算
    try{
        //替换掉表达式中的X
        QString str = express;
        str.replace('X', '*');

        //先将中缀表达式转为后缀表达式
        QString post_str = infixToPostFix(str);

        //计算后缀表达式的值
        double result = calPostFix(post_str);
        QString temp = QString::number(result, 'g', 10);            //将其转为10位有效数字

        ui->display_LineEdit->setText(temp);
    }catch(...){
        express = "Error";
        ui->display_LineEdit->setText(express);
    }
    express.clear();
}

