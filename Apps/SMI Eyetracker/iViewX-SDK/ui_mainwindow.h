/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Wed Jan 1 18:26:58 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLoad_Configuration;
    QAction *actionSave_Configuration;
    QAction *actionQuit;
    QAction *actionQuit_2;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label_4;
    QLineEdit *serverAddress;
    QLabel *label_2;
    QSpinBox *sendPort;
    QLabel *label_5;
    QLineEdit *listenAddress;
    QLabel *label_3;
    QSpinBox *receivePort;
    QLabel *label;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QSpinBox *displayLeftEdit;
    QSpinBox *displayTopEdit;
    QSpinBox *displayWidthEdit;
    QSpinBox *displayHeightEdit;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *linkButton;
    QPushButton *openDisplayButton;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(233, 294);
        actionLoad_Configuration = new QAction(MainWindow);
        actionLoad_Configuration->setObjectName(QString::fromUtf8("actionLoad_Configuration"));
        actionSave_Configuration = new QAction(MainWindow);
        actionSave_Configuration->setObjectName(QString::fromUtf8("actionSave_Configuration"));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionQuit_2 = new QAction(MainWindow);
        actionQuit_2->setObjectName(QString::fromUtf8("actionQuit_2"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        serverAddress = new QLineEdit(centralWidget);
        serverAddress->setObjectName(QString::fromUtf8("serverAddress"));

        formLayout->setWidget(0, QFormLayout::FieldRole, serverAddress);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        sendPort = new QSpinBox(centralWidget);
        sendPort->setObjectName(QString::fromUtf8("sendPort"));
        sendPort->setMinimum(1);
        sendPort->setMaximum(65535);
        sendPort->setValue(3333);

        formLayout->setWidget(1, QFormLayout::FieldRole, sendPort);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        listenAddress = new QLineEdit(centralWidget);
        listenAddress->setObjectName(QString::fromUtf8("listenAddress"));

        formLayout->setWidget(2, QFormLayout::FieldRole, listenAddress);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        receivePort = new QSpinBox(centralWidget);
        receivePort->setObjectName(QString::fromUtf8("receivePort"));
        receivePort->setMinimum(1);
        receivePort->setMaximum(65535);
        receivePort->setValue(2222);

        formLayout->setWidget(3, QFormLayout::FieldRole, receivePort);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_6);

        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_7);

        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_8);

        displayLeftEdit = new QSpinBox(centralWidget);
        displayLeftEdit->setObjectName(QString::fromUtf8("displayLeftEdit"));
        displayLeftEdit->setMaximum(999999999);

        formLayout->setWidget(4, QFormLayout::FieldRole, displayLeftEdit);

        displayTopEdit = new QSpinBox(centralWidget);
        displayTopEdit->setObjectName(QString::fromUtf8("displayTopEdit"));
        displayTopEdit->setMaximum(999999999);

        formLayout->setWidget(5, QFormLayout::FieldRole, displayTopEdit);

        displayWidthEdit = new QSpinBox(centralWidget);
        displayWidthEdit->setObjectName(QString::fromUtf8("displayWidthEdit"));
        displayWidthEdit->setMaximum(999999999);
        displayWidthEdit->setValue(1600);

        formLayout->setWidget(6, QFormLayout::FieldRole, displayWidthEdit);

        displayHeightEdit = new QSpinBox(centralWidget);
        displayHeightEdit->setObjectName(QString::fromUtf8("displayHeightEdit"));
        displayHeightEdit->setMaximum(999999999);
        displayHeightEdit->setValue(900);

        formLayout->setWidget(7, QFormLayout::FieldRole, displayHeightEdit);


        verticalLayout->addLayout(formLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        linkButton = new QPushButton(centralWidget);
        linkButton->setObjectName(QString::fromUtf8("linkButton"));

        horizontalLayout->addWidget(linkButton);

        openDisplayButton = new QPushButton(centralWidget);
        openDisplayButton->setObjectName(QString::fromUtf8("openDisplayButton"));

        horizontalLayout->addWidget(openDisplayButton);


        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 233, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionLoad_Configuration);
        menuFile->addAction(actionSave_Configuration);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit_2);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "SMI iViewX Connector", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "Load Configuration", 0, QApplication::UnicodeUTF8));
        actionSave_Configuration->setText(QApplication::translate("MainWindow", "Save Configuration", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit_2->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "iView X Server Address", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        serverAddress->setToolTip(QApplication::translate("MainWindow", "IP address of iView X computer", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        serverAddress->setText(QApplication::translate("MainWindow", "127.0.0.1", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Send Port", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        sendPort->setToolTip(QApplication::translate("MainWindow", "Port being used by this program for sending data to iView X", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_5->setText(QApplication::translate("MainWindow", "Local Listen Address", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        listenAddress->setToolTip(QApplication::translate("MainWindow", "IP address of local computer", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        listenAddress->setText(QApplication::translate("MainWindow", "127.0.0.1", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Receive Port", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        receivePort->setToolTip(QApplication::translate("MainWindow", "Port being used by this program for receiving data from iView X", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("MainWindow", "Display Left", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Display Top", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "Display Width", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Display Height", 0, QApplication::UnicodeUTF8));
        linkButton->setText(QApplication::translate("MainWindow", "Link", 0, QApplication::UnicodeUTF8));
        openDisplayButton->setText(QApplication::translate("MainWindow", "Open Display", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
