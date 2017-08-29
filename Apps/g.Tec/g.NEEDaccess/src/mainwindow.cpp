#include <qfiledialog.h>
#include <QtXml>
#include <qdebug.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const QString config_file)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	load_config(config_file);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load_config(const QString filename)
{
	QFile* xmlFile = new QFile(filename);
	if (!xmlFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Could not load XML from file " << filename;
		return;
	}
	QXmlStreamReader* xmlReader = new QXmlStreamReader(xmlFile);
	while (!xmlReader->atEnd() && !xmlReader->hasError()) {
		// Read next element
		xmlReader->readNext();
		if (xmlReader->isStartElement() && xmlReader->name() != "settings")
		{
			QStringRef elname = xmlReader->name();
			if (elname == "todo-setting-xml-element")
			{
				// TODO: Do something with the setting.
			}
		}
	}
	if (xmlReader->hasError()) {
		qDebug() << "Config file parse error "
			<< xmlReader->error()
			<< ": "
			<< xmlReader->errorString();
	}
	xmlReader->clear();
	xmlFile->close();
}