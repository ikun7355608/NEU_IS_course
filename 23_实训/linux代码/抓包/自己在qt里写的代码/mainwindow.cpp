#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<pcap.h>
#include "func_pcap.h"
#include<string.h>
#include<dialog.h>
pcap_t *handle;
char errbuf[PCAP_ERRBUF_SIZE];                          //错误信息
char *dev;                                              //网络设备接口
struct bpf_program filter;                              //BPF过滤规则
char filter_string[10]={};
int num;
bpf_u_int32 net_mask;                                   //网络掩码
bpf_u_int32 net_ip;                                     //网络地址
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dev = pcap_lookupdev(errbuf);
    pcap_lookupnet(dev, &net_ip, &net_mask, errbuf);
    handle = pcap_open_live(dev, BUFSIZ, 1, 0, errbuf);
    connect(this,SIGNAL(sendData(QString)),Dlg,SLOT(getData(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
    pcap_close(handle);
}

void MainWindow::on_radioButton_clicked()
{
    memset(filter_string,0,10);
    strcpy(filter_string,"tcp");
}

void MainWindow::on_radioButton_2_clicked()
{
    memset(filter_string,0,10);
    strcpy(filter_string,"udp");
}

void MainWindow::on_radioButton_3_clicked()
{
    memset(filter_string,0,10);
    strcpy(filter_string,"icmp");
}

void MainWindow::on_spinBox_editingFinished()
{
    num=ui->spinBox->value();
}

void MainWindow::on_pushButton_clicked()
{
    str="";
    packet_number=1;
    pcap_compile(handle, &filter, filter_string, 0, net_ip);
    pcap_setfilter(handle, &filter);
    string tmp;
    tmp=dev;
    str+="可用设备："+tmp+"\n";
    pcap_loop(handle, num, ethernet_protocol_packet_callback, NULL);
    QString qstr=QString::fromStdString(str);
    emit sendData(qstr);
    Dlg->show();

}
