#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include<dialog.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_spinBox_editingFinished();
signals:
  void sendData(QString);
private:
    Ui::MainWindow *ui;
    Dialog* Dlg=new Dialog;
};

#endif // MAINWINDOW_H
