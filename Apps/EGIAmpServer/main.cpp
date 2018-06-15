#include <QApplication>
#include "EGIAmpWindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	EGIAmpWindow w(0, std::string(argc > 1 ? argv[1] : "ampserver_config.cfg"));
	w.show();
	return a.exec();
}
