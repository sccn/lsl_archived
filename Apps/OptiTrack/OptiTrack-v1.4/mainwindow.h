#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bimap.hpp>
#include <string>
#include <vector>
#include <map>

// OptiTrack API
#include "NatNetTypes.h"
#include "NatNetClient.h"
#pragma comment (lib,"NatNetLib.lib")
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

	// OptiTrack-invoked callbacks
	static void __cdecl data_callback(sFrameOfMocapData *data, void *pUserData);	// receives data from the server
	static void __cdecl error_callback(int msgType, char *msg);					// receives NatNet error mesages

private slots:
    // config file dialog ops (from main menu)
    void load_config_dialog();
    void save_config_dialog();

    // start the OptiTrack connection
    void link_optitrack();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);

private:
    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	// called when a new frame of data is available
	void on_frame(sFrameOfMocapData *data);

	NatNetClient *nnc;								// the client connection (if != NULL, we are linked)
	boost::shared_ptr<lsl::stream_outlet> outlet;	// the current outlet

	int num_channels;								// total number of channels in the stream
	std::vector<std::string> mrk_labels;			// labels for all known markers
	std::map<std::string,int> ms_label2ofs;			// map marker set label to start offset in the channels
	boost::bimap<int,int> rig_id2slot;				// map rigid IDs to rigid slots in the channels
	std::map<int,std::string> rig_id2label;			// map rigid IDs to rigid slots in the channels

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
