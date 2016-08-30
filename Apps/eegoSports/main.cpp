#include "mainwindow.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	std::string config_file = "eegoSports_config.cfg";
	bool link = false;

	for (int k = 1; k<argc; k++) {
		if (std::string(argv[k]) == "-c" || std::string(argv[k]) == "--config")
			config_file = argv[k + 1];
		if (std::string(argv[k]) == "-l" || std::string(argv[k]) == "--link")
			link = std::string(argv[k + 1]) == "0" ? false : true;
			
	}

	QApplication a(argc, argv);
	MainWindow w(0, config_file, link);
	w.show();


	return a.exec();
}
