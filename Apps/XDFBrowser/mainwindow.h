#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "xdffile.h"
#include <QThread>
#include <QStringListModel>
#include <QItemSelection>

class StringList : public QStringListModel
{
public:
  void append (const QString& string){
    insertRows(rowCount(), 1);
    setData(index(rowCount()-1), string);
  }
  StringList& operator<<(const QString& string){
    append(string);
    return *this;
  }
};


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
    void on_actionOpen_triggered();
	void on_actionClose_triggered();
	void on_actionSave_As_triggered(); 
	void on_actionSave_triggered(); 
	void onTextChanged();
	void handleSelectionChanged(const QItemSelection& selection);
	void errorDialog(QString errorMessage);

private:
    Ui::MainWindow *ui;
    std::shared_ptr<XDFfile> xdfFile;
    std::shared_ptr<StringList> chunkNameList;
};

  class OpenerThread : public QThread
 {
 Q_OBJECT
 public:
	 OpenerThread() {}


    std::shared_ptr<XDFfile> xdfFile;
    std::shared_ptr<StringList> chunkNameList;
	QProgressBar *progressBar;


signals:
	void errorMessageSignal(QString errorMessage);

private:
	 

     void run() {

		try {
			xdfFile->open(progressBar);

			for(int i=0; i<xdfFile->getNChunks(); i++) {
				chunkNameList->append(xdfFile->getChunkName(i));
			}

		} catch (const IOException &ex) {
			emit errorMessageSignal(ex.what());
		}

     }

 };

   class SaverThread : public QThread
 {
  Q_OBJECT
 public:
    std::shared_ptr<XDFfile> xdfFile;
	QString xdfFileName;
	QProgressBar *progressBar;


signals:
	void errorMessageSignal(QString errorMessage);

 private:	 

     void run() {
		 try {
			xdfFile->saveAs(xdfFileName, progressBar);
		 } catch (const IOException &ex) {
			emit errorMessageSignal(ex.what());
		 }
     }

 };



#endif // MAINWINDOW_H
