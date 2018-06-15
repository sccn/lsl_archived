#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QCommandLineParser parser;
	parser.setApplicationDescription("LabStreamingLayer interface for g.tec g.NEEDaccess.");
	parser.addHelpOption();
	parser.addVersionOption();
	QCommandLineOption configFileOption(QStringList() << "c" << "config",
		QCoreApplication::translate("main", "Load configuration from <config>."),
		QCoreApplication::translate("main", "config"),
		default_config_fname);
	parser.addOption(configFileOption);
	parser.process(a);
	QString configFilename = parser.value(configFileOption);
	MainWindow w(0, configFilename);
    w.show();

	return a.exec();
}
