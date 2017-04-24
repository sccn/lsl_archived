#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// boost
#include <boost/thread.hpp>

// Qt
#include <QtWidgets/QMainWindow>


namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, const std::string &config_file);
	
    ~MainWindow();

private slots:    
    // config file dialog ops (from main menu)
    void load_config_dialog();
    void save_config_dialog();

	// start connection
    void link();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);

	// status messages
	void statusMessage(QString msg);

signals:
	void sendMessage(QString msg);

private:

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	void listen(int sps);
	boost::shared_ptr<boost::thread> listen_thread_;
	void connect(QString serverIP, QString port);
	boost::shared_ptr<boost::thread> connect_thread_;

	bool stop_;
	boost::mutex mtx_;

	Ui::MainWindow *ui;									// window pointer
};

#endif // MAINWINDOW_H
