#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gdsthread.h"

const QString default_config_fname = "gneedaccess_config.cfg";

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0,
		const QString config_file = default_config_fname);
    ~MainWindow();

private:
	void load_config(const QString filename);
	void save_config(const QString filename);

    Ui::MainWindow *ui;
	GDSThread m_thread;
};

#endif // MAINWINDOW_H
