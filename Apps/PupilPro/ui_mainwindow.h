/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue Jan 19 11:16:30 2016
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
    QLineEdit *serverIP;
    QLabel *pupilProPortLabel;
    QLineEdit *port;
    QLabel *label_2;
    QLabel *connectionStatus;
    QLabel *pupilProFpsLabel;
    QLineEdit *fps;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *linkButton;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(379, 230);
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
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        serverIP = new QLineEdit(centralWidget);
        serverIP->setObjectName(QString::fromUtf8("serverIP"));

        formLayout->setWidget(0, QFormLayout::FieldRole, serverIP);

        pupilProPortLabel = new QLabel(centralWidget);
        pupilProPortLabel->setObjectName(QString::fromUtf8("pupilProPortLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, pupilProPortLabel);

        port = new QLineEdit(centralWidget);
        port->setObjectName(QString::fromUtf8("port"));

        formLayout->setWidget(2, QFormLayout::FieldRole, port);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_2);

        connectionStatus = new QLabel(centralWidget);
        connectionStatus->setObjectName(QString::fromUtf8("connectionStatus"));

        formLayout->setWidget(4, QFormLayout::FieldRole, connectionStatus);

        pupilProFpsLabel = new QLabel(centralWidget);
        pupilProFpsLabel->setObjectName(QString::fromUtf8("pupilProFpsLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, pupilProFpsLabel);

        fps = new QLineEdit(centralWidget);
        fps->setObjectName(QString::fromUtf8("fps"));

        formLayout->setWidget(3, QFormLayout::FieldRole, fps);


        verticalLayout->addLayout(formLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        linkButton = new QPushButton(centralWidget);
        linkButton->setObjectName(QString::fromUtf8("linkButton"));

        horizontalLayout->addWidget(linkButton);


        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 379, 22));
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
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "PupilPro-LSL Connector", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "Load Configuration", 0, QApplication::UnicodeUTF8));
        actionSave_Configuration->setText(QApplication::translate("MainWindow", "Save Configuration", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit_2->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "PupilPro Address", 0, QApplication::UnicodeUTF8));
        serverIP->setText(QString());
        pupilProPortLabel->setText(QApplication::translate("MainWindow", "PupilPro Port", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Connection Status", 0, QApplication::UnicodeUTF8));
        connectionStatus->setText(QString());
        pupilProFpsLabel->setText(QApplication::translate("MainWindow", "PupilPro fps", 0, QApplication::UnicodeUTF8));
        linkButton->setText(QApplication::translate("MainWindow", "Link", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
