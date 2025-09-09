#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_current_file_path.clear();

    //设置快捷键
    ui->new_file_Action->setShortcut(QKeySequence::New);    //使用标准快捷键枚举值
    ui->open_file_Action->setShortcut(QKeySequence::Open);
    ui->save_file_Action->setShortcut(QKeySequence::Save);
    ui->save_another_file_Action->setShortcut(QKeySequence("Ctrl+Shift+S"));    //自定义快捷键

    //连接信号与槽
    connect(ui->new_file_Action, &QAction::triggered, this, &MainWindow::newTextAction);
    connect(ui->open_file_Action, &QAction::triggered, this, &MainWindow::openTextAction);
    connect(ui->save_file_Action, &QAction::triggered, this, &MainWindow::saveTextAction);
    connect(ui->save_another_file_Action, &QAction::triggered, this, &MainWindow::saveAnotherAction);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier){
        switch(event->key()){
        case Qt::Key_N: this->newTextAction();  break;
        case Qt::Key_O: this->openTextAction(); break;
        case Qt::Key_S:
        {
            if(event->modifiers() & Qt::ShiftModifier){
                this->saveAnotherAction();
            }else{
                this->saveTextAction();
            }
            break;
        }
        default:
            QMainWindow::keyPressEvent(event);  // 调用基类处理
        }
    }else{
        QMainWindow::keyPressEvent(event);  // 调用基类处理
    }
}

void MainWindow::newTextAction()
{
    ui->textEdit->clear();
    m_current_file_path.clear();
    this->setWindowTitle("新建文本文档.txt");
}

void MainWindow::openTextAction()
{
    QString current_path = QCoreApplication::applicationFilePath();             //获取当前文件路径
    QString file_Name = QFileDialog::getOpenFileName(this, tr("打开文件"),
                                                    current_path,
                                                    tr("Text files (*.txt *.c *.cpp)"));
    if(file_Name.isNull()){
        QMessageBox::warning(this, "警告","请选择一个文件");
    }else{
        QFile file(file_Name);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file);
        QString content = in.readAll();
        ui->textEdit->setText(content);
        file.close();

        //记录当前路径
        m_current_file_path = file_Name;

        this->setWindowTitle(QFileInfo(file_Name).fileName());
    }
}

void MainWindow::saveTextAction()
{
    //如果当前没有打开文件，那么就另存为
    if(m_current_file_path.isEmpty()){
        this->saveAnotherAction();
        return;
    }

    QFile file(m_current_file_path);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this, "错误","无法保存文件");
        return;
    }

    QTextStream out(&file);
    out<<ui->textEdit->toPlainText();
    file.close();

    QMessageBox::information(this, "成功","保存文件");
}

void MainWindow::saveAnotherAction()
{
    QString current_path = QCoreApplication::applicationFilePath();             //获取当前文件路径
    QString save_path = QFileDialog::getSaveFileName(this, tr("保存文件"),
                                                    current_path,
                                                    tr("Text files (*.txt *.c *.cpp)"));
    if(save_path.isNull()){
        QMessageBox::warning(this, "警告","请选择保存位置");
    }else{
        QFile file(save_path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::warning(this, "错误", "无法保存文件");
            return;
        }

        QTextStream out(&file);
        out<<ui->textEdit->toPlainText();
        file.close();

        //更新当前路径
        m_current_file_path = save_path;

        //设置标题
        this->setWindowTitle(QFileInfo(save_path).fileName());

        QMessageBox::information(this, "成功","保存文件");
    }
}
