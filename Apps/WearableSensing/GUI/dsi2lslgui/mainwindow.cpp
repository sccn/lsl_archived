#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>

#include <unistd.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set LD_LIBRARY_PATH
    char cdir[256];
    std:setenv("LD_LIBRARY_PATH",getcwd(cdir, 256), 1);

    ui->nameLineEdit->setText("WS-default");
    ui->montageLineEdit->setText("(use default)");
    ui->referenceLineEdit->setText("(use default)");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Start");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Stop");
    ui->statusBar->setVisible(false);
    this->ui->statusBar->showMessage("Streaming...");
    this->progressBar = new QProgressBar(this);
    ui->statusBar->addPermanentWidget(this->progressBar, 1);
    this->progressBar->setTextVisible(false);

    #ifdef WIN32
    this->program = "dsi2lsl.exe";
    #else
    this->program = "./dsi2lsl";
    #endif

    // Create a brand new streamer
    this->streamer = new QProcess(this);
    this->streamer->setProcessChannelMode(QProcess::MergedChannels);
}



MainWindow::~MainWindow()
{
    this->on_buttonBox_rejected();
    delete ui;
}




void MainWindow::on_buttonBox_accepted()
{
    if(this->streamer != NULL)
        this->on_buttonBox_rejected();

    // Set input arguments to the streamer
    QStringList arguments = this->parseArguments();
    this->streamer->start(this->program, arguments);
    connect(this->streamer, SIGNAL(readyReadStandardOutput()), this, SLOT(writeToConsole()));
    this->counter = 0;
    this->timerId = this->startTimer(1000);
}


void MainWindow::timerEvent(QTimerEvent *event)
{
    if(this->streamer->state()==QProcess::NotRunning)
    {
        this->on_buttonBox_rejected();
        return;
    }

    if(!this->ui->statusBar->isVisible())
        this->ui->statusBar->setVisible(true);
    this->counter += 33;
    if(this->counter > 100)
        this->counter = 0;
    this->progressBar->setValue(this->counter);
    this->ui->statusBar->showMessage("Streaming...");
}


void MainWindow::writeToConsole()
{
    while(this->streamer->canReadLine()){
        this->ui->console->append(this->streamer->readLine());
    }
}



void MainWindow::on_buttonBox_rejected()
{
    if(this->streamer != NULL){
        this->streamer->close();
        this->ui->console->append("Streamer will exit now. Good bye!");
        this->killTimer(this->timerId);
        this->counter = 0;
        this->ui->statusBar->setVisible(false);
    }

}


QStringList MainWindow::parseArguments()
{
    QStringList arguments;

    arguments << (this->port+this->ui->portLineEdit->text().simplified()).toStdString().c_str()
              << (this->lslStream+this->ui->nameLineEdit->text().simplified()).toStdString().c_str();

    if(ui->montageLineEdit->text().simplified().compare(this->defaultValule))
        arguments << (this->montage+this->ui->montageLineEdit->text().simplified()).toStdString().c_str();

    if(ui->referenceLineEdit->text().simplified().compare(this->defaultValule))
        arguments << (this->reference+this->ui->referenceLineEdit->text().simplified()).toStdString().c_str();

    return arguments;
}
