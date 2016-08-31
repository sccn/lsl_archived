#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <string>
#include <vector>

// PhaseSpace API
#include "owl.h"
#pragma comment (lib,"libowlsock.lib")
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

    // start the phasespace connection
    void link_phasespace();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);
private:
    // background data reader thread
    void read_thread(float srate, int interpolation, std::vector<struct OWLCamera> cameras,
                     std::vector<struct OWLMarker> markers, std::vector<struct OWLRigid> rigids);

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	// misc data
	std::vector<std::string> marker_names, rigid_names;
	std::string setup_name;
	std::string server_name;
	bool stream_per_rigid;
	bool stop_;

    boost::shared_ptr<boost::thread> reader_thread_;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
