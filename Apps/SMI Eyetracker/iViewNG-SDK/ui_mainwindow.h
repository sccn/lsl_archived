/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sat Dec 14 16:16:33 2013
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
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
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
    QLineEdit *serverPort;
    QLabel *label_5;
    QLineEdit *licenseKey;
    QLabel *label_3;
    QComboBox *deviceType;
    QLabel *label;
    QComboBox *samplingRate;
    QLabel *label_6;
    QComboBox *sceneResolution;
    QLabel *label_7;
    QComboBox *sceneColorSpace;
    QLabel *label_8;
    QComboBox *eyeResolution;
    QLabel *label_9;
    QComboBox *eyeSides;
    QLabel *label_10;
    QSpinBox *eyeSamplingRate;
    QCheckBox *addCompressedSceneStream;
    QLabel *label_11;
    QSpacerItem *verticalSpacer;
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
        MainWindow->resize(276, 373);
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

        serverAddress = new QLineEdit(centralWidget);
        serverAddress->setObjectName(QString::fromUtf8("serverAddress"));

        formLayout->setWidget(0, QFormLayout::FieldRole, serverAddress);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        serverPort = new QLineEdit(centralWidget);
        serverPort->setObjectName(QString::fromUtf8("serverPort"));

        formLayout->setWidget(1, QFormLayout::FieldRole, serverPort);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        licenseKey = new QLineEdit(centralWidget);
        licenseKey->setObjectName(QString::fromUtf8("licenseKey"));

        formLayout->setWidget(2, QFormLayout::FieldRole, licenseKey);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        deviceType = new QComboBox(centralWidget);
        deviceType->setObjectName(QString::fromUtf8("deviceType"));

        formLayout->setWidget(3, QFormLayout::FieldRole, deviceType);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label);

        samplingRate = new QComboBox(centralWidget);
        samplingRate->setObjectName(QString::fromUtf8("samplingRate"));

        formLayout->setWidget(4, QFormLayout::FieldRole, samplingRate);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_6);

        sceneResolution = new QComboBox(centralWidget);
        sceneResolution->setObjectName(QString::fromUtf8("sceneResolution"));

        formLayout->setWidget(5, QFormLayout::FieldRole, sceneResolution);

        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_7);

        sceneColorSpace = new QComboBox(centralWidget);
        sceneColorSpace->setObjectName(QString::fromUtf8("sceneColorSpace"));

        formLayout->setWidget(6, QFormLayout::FieldRole, sceneColorSpace);

        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_8);

        eyeResolution = new QComboBox(centralWidget);
        eyeResolution->setObjectName(QString::fromUtf8("eyeResolution"));

        formLayout->setWidget(7, QFormLayout::FieldRole, eyeResolution);

        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout->setWidget(8, QFormLayout::LabelRole, label_9);

        eyeSides = new QComboBox(centralWidget);
        eyeSides->setObjectName(QString::fromUtf8("eyeSides"));

        formLayout->setWidget(8, QFormLayout::FieldRole, eyeSides);

        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout->setWidget(9, QFormLayout::LabelRole, label_10);

        eyeSamplingRate = new QSpinBox(centralWidget);
        eyeSamplingRate->setObjectName(QString::fromUtf8("eyeSamplingRate"));
        eyeSamplingRate->setValue(30);

        formLayout->setWidget(9, QFormLayout::FieldRole, eyeSamplingRate);

        addCompressedSceneStream = new QCheckBox(centralWidget);
        addCompressedSceneStream->setObjectName(QString::fromUtf8("addCompressedSceneStream"));

        formLayout->setWidget(10, QFormLayout::FieldRole, addCompressedSceneStream);

        label_11 = new QLabel(centralWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout->setWidget(10, QFormLayout::LabelRole, label_11);


        verticalLayout->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(20, 4, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

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
        menuBar->setGeometry(QRect(0, 0, 276, 18));
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
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "SMI iViewNG Connector", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "Load Configuration", 0, QApplication::UnicodeUTF8));
        actionSave_Configuration->setText(QApplication::translate("MainWindow", "Save Configuration", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit_2->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Server Address", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        serverAddress->setToolTip(QApplication::translate("MainWindow", "IP address of iViewNG server machine; if set to \"launch process\", a minimal server instance will be started in the background.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        serverAddress->setText(QApplication::translate("MainWindow", "(launch process)", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Server Port", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        serverPort->setToolTip(QApplication::translate("MainWindow", "Note: the default port is 1100", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        serverPort->setText(QApplication::translate("MainWindow", "(use default)", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "License Key", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        licenseKey->setToolTip(QApplication::translate("MainWindow", "Not all features might be available with the default license key, but as of 2013 there is no notable restriction.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        licenseKey->setText(QApplication::translate("MainWindow", "(use default)", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Device Type", 0, QApplication::UnicodeUTF8));
        deviceType->clear();
        deviceType->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "ETG 1.x", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "ETG 2.x", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        deviceType->setToolTip(QApplication::translate("MainWindow", "ETG stands for eye tracking glasses -- see the inside of your glasses frame for the version number or check the vendor's website.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("MainWindow", "Gaze Sampling Rate", 0, QApplication::UnicodeUTF8));
        samplingRate->clear();
        samplingRate->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "30", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "60", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        samplingRate->setToolTip(QApplication::translate("MainWindow", "Higher sampling rates might run at lower resolution. Note that not all trackers may support higher sampling rates.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_6->setText(QApplication::translate("MainWindow", "Scene Camera Resolution", 0, QApplication::UnicodeUTF8));
        sceneResolution->clear();
        sceneResolution->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "(do not stream)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "160x120", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "320x240", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "400x300", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "640x480", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "960x720", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1280x720", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1280x960", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1920x1080", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        sceneResolution->setToolTip(QApplication::translate("MainWindow", "This stream is uncompressed -- do not record it to disk! On a low-bandwidth network, try to use the lowest possible resolution.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_7->setText(QApplication::translate("MainWindow", "Scene Camera Color Space", 0, QApplication::UnicodeUTF8));
        sceneColorSpace->clear();
        sceneColorSpace->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Grayscale", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "FullColor", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        sceneColorSpace->setToolTip(QApplication::translate("MainWindow", "Use grayscale to conserve bandwidth.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_8->setText(QApplication::translate("MainWindow", "Eye Camera Resolution", 0, QApplication::UnicodeUTF8));
        eyeResolution->clear();
        eyeResolution->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "(do not stream)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "160x120", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "320x240", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "400x300", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "640x480", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        eyeResolution->setToolTip(QApplication::translate("MainWindow", "These streams are uncompressed -- do not record them to disk! On a low-bandwidth network, try to use the lowest possible resolution.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_9->setText(QApplication::translate("MainWindow", "Eye Camera Side", 0, QApplication::UnicodeUTF8));
        eyeSides->clear();
        eyeSides->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "left eye", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "right eye", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "both eyes", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        eyeSides->setToolTip(QApplication::translate("MainWindow", "Select which of the eye streams you want to generate.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_10->setText(QApplication::translate("MainWindow", "Eye Camera Rate", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        eyeSamplingRate->setToolTip(QApplication::translate("MainWindow", "Use a low sampling rate to conserve bandwidth", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        addCompressedSceneStream->setToolTip(QApplication::translate("MainWindow", "If checked, a compressed scene-image stream will be generated that is mostly useful for recording to disk.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        addCompressedSceneStream->setText(QApplication::translate("MainWindow", "(check)", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainWindow", "Compressed Scene Stream", 0, QApplication::UnicodeUTF8));
        linkButton->setText(QApplication::translate("MainWindow", "Link", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
