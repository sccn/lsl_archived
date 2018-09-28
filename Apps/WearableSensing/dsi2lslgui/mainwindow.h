#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtGui>


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
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void writeToConsole();
    QStringList parseArguments();
    void timerEvent(QTimerEvent *event);

private:
    Ui::MainWindow *ui;
    QProcess *streamer;
    QString program = "./dsi2lsl";
    QString port = "--port=";
    QString lslStream = "--lsl-stream-name=";
    QString montage = "--montage=";
    QString reference = "--reference=";
    QString defaultValule = "(use default)";
    int timerId;
    int counter = 0;
    QProgressBar *progressBar;
};

#endif // MAINWINDOW_H
