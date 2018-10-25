#include "widget.h"
#include <QApplication>
#include <QTextBlock>

Widget* msgHandlePointer;
void getMsgHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    QString str;
    switch (type) {
    case QtDebugMsg:
        str = QString::asprintf("Debug: %s (%s:%u, %s)", localMsg.constData(), context.file,
                                context.line, context.function);
        break;
    case QtInfoMsg:
        // str = QString::asprintf("Info: %s (%s:%u, %s)", localMsg.constData(),
        // context.file, context.line, context.function);
        str = QString::asprintf("Info: %s", localMsg.constData());
        break;
    case QtWarningMsg:
        str = QString::asprintf("Warning: %s (%s:%u, %s)", localMsg.constData(), context.file,
                                context.line, context.function);
        break;
    case QtCriticalMsg:
        str = QString::asprintf("Critical: %s (%s:%u, %s)", localMsg.constData(), context.file,
                                context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line,
                context.function);
        abort();
    default:
        str = QString::asprintf("Msg No%u: %s (%s:%u, %s)", type, localMsg.constData(),
                                context.file, context.line, context.function);
    }
    fprintf(stderr, "%s", str.toStdString().c_str());
    msgHandlePointer->displayMsg(str);
}
int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    // Q_DECLARE_METATYPE(QTextBlock);
    qRegisterMetaType<QTextBlock>("QTextBlock");
    // Q_DECLARE_METATYPE(QTextCursor);
    qRegisterMetaType<QTextCursor>("QTextCursor");
    Widget w;
    msgHandlePointer = &w;
    qInstallMessageHandler(getMsgHandler);
    w.show();

    return a.exec();
}
