#ifndef WIDGET_H
#define WIDGET_H

#include "NeuroscanAcquireProtocol.h"
#include "nsatransporter.h"
#include <QMutex>
#include <QMutexLocker>
#include <QTcpSocket>
#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget* parent = 0);
    ~Widget();

public:
    void displayMsg(QString);

private slots:
    void on_pushButton_clicked();
    void thread_stop();
    void display(QString);

signals:
    void set_host(const QString&, const int&);
    void transmit_msg(QString);
    void send_code(QByteArray);

private:
    Ui::Widget* ui;

private:
    QThread* thread;
    NsaTransporter* nsa;

    //QMutex mutex;
};

#endif // WIDGET_H
