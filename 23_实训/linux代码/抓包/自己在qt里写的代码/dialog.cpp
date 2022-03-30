#include "dialog.h"
#include "ui_dialog.h"
#include<QString>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

}
void Dialog::getData(QString str1)
{
  ui->textEdit->setText(str1);
}
Dialog::~Dialog()
{
    delete ui;
}
