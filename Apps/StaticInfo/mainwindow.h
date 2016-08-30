#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

// LSL API
#include <lsl_cpp.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent, const std::string &config_file);
    ~MainWindow();
    
private slots:
    // config file dialog ops (from main menu)
    void load_config_dialog();
    void save_config_dialog();

    // file browse action
    void pick_file_dialog();

    // start the connection
    void on_link();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);
private:

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	boost::shared_ptr<lsl::stream_outlet> outlet;	// our stream outlet

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
