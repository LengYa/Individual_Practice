#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event) override;              //实现快捷键

private slots:
    void newTextAction();               //新建文档
    void openTextAction();              //打开文档
    void saveTextAction();              //保存文档
    void saveAnotherAction();           //文档另存为

private:
    Ui::MainWindow *ui;
    QString m_current_file_path;        //存储当前的文件路径
};
#endif // MAINWINDOW_H
