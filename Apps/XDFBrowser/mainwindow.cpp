#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QThread>
#include <QMessageBox>

#include "xdffile.h"

MainWindow::MainWindow(QWidget *parent, const char* filename) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
	if(filename) loadfile(QString::fromLatin1(filename));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleSelectionChanged(const QItemSelection& selection) {
	if(selection.indexes().isEmpty()) {
		
	} else {
		int selectionAsInt = selection.indexes().first().row();
		ui->textEdit->setPlainText(xdfFile->getChunkText(selectionAsInt));
	}
}

void MainWindow::onTextChanged() {
	if(xdfFile)
		xdfFile->setChunkText(ui->listView->selectionModel()->currentIndex().row(),ui->textEdit->toPlainText());

}

void MainWindow::on_actionSave_As_triggered() 
{
	if(!xdfFile) return;

	 QString xdfFileName = QFileDialog::getSaveFileName(this, tr("Save XDF File As"), "",tr("*.xdf"));

	 if(!xdfFileName.isNull()) {
		
		SaverThread *workerThread = new SaverThread;
		workerThread->xdfFile = xdfFile;
		workerThread->xdfFileName = xdfFileName;
		workerThread->progressBar = ui->progressBar;
		connect(workerThread, SIGNAL(errorMessageSignal(QString)), this, SLOT(errorDialog(QString)));
		connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
		workerThread->start();

		this->setWindowTitle(xdfFileName);
	 }
}

void MainWindow::on_actionSave_triggered()
{
	//try invoke method with this function?
//QMessageBox::critical(NULL, tr("Critical"), tr("Critical message text"));
	
	if(!xdfFile) return;

	SaverThread *workerThread = new SaverThread;
	workerThread->xdfFile = xdfFile;
	workerThread->xdfFileName = xdfFile->getFileName();
	workerThread->progressBar = ui->progressBar;
	connect(workerThread, SIGNAL(errorMessageSignal(QString)), this, SLOT(errorDialog(QString)));
	connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
	workerThread->start();
	
	//xdfFile->saveAs(xdfFile->getFileName()),ui->progressBar);
	
}

void MainWindow::on_actionOpen_triggered()
{

    QString xdfFileName = QFileDialog::getOpenFileName(this, tr("Open XDF File"), tr("*.xdf"));
	loadfile(xdfFileName);
}

void MainWindow::loadfile(QString xdfFileName) {
	if(!xdfFileName.isNull()) {
		chunkNameList.reset(new StringList());
		ui->listView->setModel(chunkNameList.get());
		connect(ui->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
			this, SLOT(handleSelectionChanged(QItemSelection)));
		
		xdfFile.reset(new XDFfile(xdfFileName));
    
		OpenerThread *workerThread = new OpenerThread;
		workerThread->xdfFile = xdfFile;
		workerThread->chunkNameList = chunkNameList;
		workerThread->progressBar = ui->progressBar;
		connect(workerThread, SIGNAL(errorMessageSignal(QString)), this, SLOT(errorDialog(QString)));
		connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
	    workerThread->start();
		this->setWindowTitle(xdfFileName);
	}
}

void MainWindow::on_actionClose_triggered()
{
	
	if(xdfFile) {
		chunkNameList.reset();		
		xdfFile.get()->close();
		xdfFile.reset();
		ui->textEdit->setPlainText(QString(""));
	}


}

void MainWindow::errorDialog(QString errorMessage) {
	QMessageBox::critical(this, tr("Error"), errorMessage);
}


