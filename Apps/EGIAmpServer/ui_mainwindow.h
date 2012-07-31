/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon Jul 30 20:48:28 2012
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
#include <QtGui/QGroupBox>
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
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_4;
    QLineEdit *serverAddress;
    QLabel *label_5;
    QSpinBox *commandPort;
    QLabel *label_6;
    QSpinBox *notificationPort;
    QLabel *label_7;
    QSpinBox *dataPort;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout_2;
    QLabel *label_3;
    QSpinBox *amplifierId;
    QLabel *label;
    QSpinBox *samplingRate;
    QLabel *label_2;
    QSpinBox *channelCount;
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
        MainWindow->resize(282, 331);
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
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        formLayout = new QFormLayout(groupBox);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        serverAddress = new QLineEdit(groupBox);
        serverAddress->setObjectName(QString::fromUtf8("serverAddress"));

        formLayout->setWidget(0, QFormLayout::FieldRole, serverAddress);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_5);

        commandPort = new QSpinBox(groupBox);
        commandPort->setObjectName(QString::fromUtf8("commandPort"));
        commandPort->setMaximum(65535);
        commandPort->setValue(9877);

        formLayout->setWidget(1, QFormLayout::FieldRole, commandPort);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_6);

        notificationPort = new QSpinBox(groupBox);
        notificationPort->setObjectName(QString::fromUtf8("notificationPort"));
        notificationPort->setMaximum(65535);
        notificationPort->setValue(9878);

        formLayout->setWidget(2, QFormLayout::FieldRole, notificationPort);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_7);

        dataPort = new QSpinBox(groupBox);
        dataPort->setObjectName(QString::fromUtf8("dataPort"));
        dataPort->setMaximum(65535);
        dataPort->setValue(9879);

        formLayout->setWidget(3, QFormLayout::FieldRole, dataPort);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        formLayout_2 = new QFormLayout(groupBox_2);
        formLayout_2->setSpacing(6);
        formLayout_2->setContentsMargins(11, 11, 11, 11);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_3);

        amplifierId = new QSpinBox(groupBox_2);
        amplifierId->setObjectName(QString::fromUtf8("amplifierId"));
        amplifierId->setMinimum(0);
        amplifierId->setMaximum(32);
        amplifierId->setValue(0);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, amplifierId);

        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label);

        samplingRate = new QSpinBox(groupBox_2);
        samplingRate->setObjectName(QString::fromUtf8("samplingRate"));
        samplingRate->setMinimum(1);
        samplingRate->setMaximum(40000);
        samplingRate->setValue(1000);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, samplingRate);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMinimumSize(QSize(131, 0));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_2);

        channelCount = new QSpinBox(groupBox_2);
        channelCount->setObjectName(QString::fromUtf8("channelCount"));
        channelCount->setMinimum(1);
        channelCount->setMaximum(512);
        channelCount->setValue(280);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, channelCount);


        verticalLayout->addWidget(groupBox_2);

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
        menuBar->setGeometry(QRect(0, 0, 282, 18));
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
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "EGI AmpServer Connector", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "Load Configuration", 0, QApplication::UnicodeUTF8));
        actionSave_Configuration->setText(QApplication::translate("MainWindow", "Save Configuration", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit_2->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Network Settings", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "AmpServer Address", 0, QApplication::UnicodeUTF8));
        serverAddress->setText(QApplication::translate("MainWindow", "172.16.2.249", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "AmpServer Command Port", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "AmpServer Notification Port", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "AmpServer Data Port", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Recording Settings", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Amplifier Id", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Sampling Rate", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Number of Channels", 0, QApplication::UnicodeUTF8));
        linkButton->setText(QApplication::translate("MainWindow", "Link", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
