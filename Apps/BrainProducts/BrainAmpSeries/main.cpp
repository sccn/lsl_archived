#include <QApplication>
#include "mainwindow.h"
#include <string>

int main(int argc, char *argv[])
{
    // determine the startup config file...
    const char* config_file = "BrainAmpSeries_config.cfg";
    for (int k=1;k<argc;k++)
        if (std::string(argv[k]) == "-c" || std::string(argv[k]) == "--config")
            config_file = argv[k+1];

    QApplication a(argc, argv);
	MainWindow w(nullptr, config_file);
	w.show();
    return a.exec();
}
