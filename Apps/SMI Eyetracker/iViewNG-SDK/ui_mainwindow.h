/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri Jan 24 18:23:59 2014
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
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_4;
    QLineEdit *serverAddress;
    QLabel *label_2;
    QLineEdit *serverPort;
    QLabel *label_5;
    QLineEdit *licenseKey;
    QLabel *label_3;
    QComboBox *deviceType;
    QGroupBox *groupBox_3;
    QFormLayout *formLayout_3;
    QLabel *label_8;
    QComboBox *eyeResolution;
    QLabel *label_10;
    QComboBox *eyeSamplingRate;
    QLabel *label_9;
    QComboBox *eyeTrackSides;
    QLabel *label_14;
    QCheckBox *addLeftEyeVideo;
    QLabel *label_15;
    QCheckBox *addRightEyeVideo;
    QLabel *label_12;
    QCheckBox *addGazeStream;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout_2;
    QLabel *label;
    QComboBox *sceneSamplingRate;
    QLabel *label_6;
    QComboBox *sceneResolution;
    QLabel *label_7;
    QComboBox *sceneColorSpace;
    QLabel *label_16;
    QComboBox *sceneWhiteBalance;
    QLabel *label_13;
    QCheckBox *addUncompressedSceneStream;
    QLabel *label_11;
    QCheckBox *addCompressedSceneStream;
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
        MainWindow->resize(372, 596);
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
        verticalSpacer = new QSpacerItem(20, 4, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

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

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        serverPort = new QLineEdit(groupBox);
        serverPort->setObjectName(QString::fromUtf8("serverPort"));

        formLayout->setWidget(1, QFormLayout::FieldRole, serverPort);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        licenseKey = new QLineEdit(groupBox);
        licenseKey->setObjectName(QString::fromUtf8("licenseKey"));

        formLayout->setWidget(2, QFormLayout::FieldRole, licenseKey);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        deviceType = new QComboBox(groupBox);
        deviceType->setObjectName(QString::fromUtf8("deviceType"));

        formLayout->setWidget(3, QFormLayout::FieldRole, deviceType);


        verticalLayout->addWidget(groupBox);

        groupBox_3 = new QGroupBox(centralWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        formLayout_3 = new QFormLayout(groupBox_3);
        formLayout_3->setSpacing(6);
        formLayout_3->setContentsMargins(11, 11, 11, 11);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_8);

        eyeResolution = new QComboBox(groupBox_3);
        eyeResolution->setObjectName(QString::fromUtf8("eyeResolution"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, eyeResolution);

        label_10 = new QLabel(groupBox_3);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_10);

        eyeSamplingRate = new QComboBox(groupBox_3);
        eyeSamplingRate->setObjectName(QString::fromUtf8("eyeSamplingRate"));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, eyeSamplingRate);

        label_9 = new QLabel(groupBox_3);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_3->setWidget(2, QFormLayout::LabelRole, label_9);

        eyeTrackSides = new QComboBox(groupBox_3);
        eyeTrackSides->setObjectName(QString::fromUtf8("eyeTrackSides"));

        formLayout_3->setWidget(2, QFormLayout::FieldRole, eyeTrackSides);

        label_14 = new QLabel(groupBox_3);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        formLayout_3->setWidget(3, QFormLayout::LabelRole, label_14);

        addLeftEyeVideo = new QCheckBox(groupBox_3);
        addLeftEyeVideo->setObjectName(QString::fromUtf8("addLeftEyeVideo"));

        formLayout_3->setWidget(3, QFormLayout::FieldRole, addLeftEyeVideo);

        label_15 = new QLabel(groupBox_3);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        formLayout_3->setWidget(4, QFormLayout::LabelRole, label_15);

        addRightEyeVideo = new QCheckBox(groupBox_3);
        addRightEyeVideo->setObjectName(QString::fromUtf8("addRightEyeVideo"));

        formLayout_3->setWidget(4, QFormLayout::FieldRole, addRightEyeVideo);

        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        formLayout_3->setWidget(5, QFormLayout::LabelRole, label_12);

        addGazeStream = new QCheckBox(groupBox_3);
        addGazeStream->setObjectName(QString::fromUtf8("addGazeStream"));
        addGazeStream->setChecked(true);

        formLayout_3->setWidget(5, QFormLayout::FieldRole, addGazeStream);


        verticalLayout->addWidget(groupBox_3);

        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        formLayout_2 = new QFormLayout(groupBox_2);
        formLayout_2->setSpacing(6);
        formLayout_2->setContentsMargins(11, 11, 11, 11);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label);

        sceneSamplingRate = new QComboBox(groupBox_2);
        sceneSamplingRate->setObjectName(QString::fromUtf8("sceneSamplingRate"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, sceneSamplingRate);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_6);

        sceneResolution = new QComboBox(groupBox_2);
        sceneResolution->setObjectName(QString::fromUtf8("sceneResolution"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, sceneResolution);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_7);

        sceneColorSpace = new QComboBox(groupBox_2);
        sceneColorSpace->setObjectName(QString::fromUtf8("sceneColorSpace"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, sceneColorSpace);

        label_16 = new QLabel(groupBox_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_16);

        sceneWhiteBalance = new QComboBox(groupBox_2);
        sceneWhiteBalance->setObjectName(QString::fromUtf8("sceneWhiteBalance"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, sceneWhiteBalance);

        label_13 = new QLabel(groupBox_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_13);

        addUncompressedSceneStream = new QCheckBox(groupBox_2);
        addUncompressedSceneStream->setObjectName(QString::fromUtf8("addUncompressedSceneStream"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, addUncompressedSceneStream);

        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout_2->setWidget(5, QFormLayout::LabelRole, label_11);

        addCompressedSceneStream = new QCheckBox(groupBox_2);
        addCompressedSceneStream->setObjectName(QString::fromUtf8("addCompressedSceneStream"));

        formLayout_2->setWidget(5, QFormLayout::FieldRole, addCompressedSceneStream);


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
        groupBox->raise();
        groupBox_2->raise();
        groupBox_3->raise();
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 372, 18));
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

        eyeResolution->setCurrentIndex(0);
        eyeSamplingRate->setCurrentIndex(1);
        eyeTrackSides->setCurrentIndex(2);
        sceneSamplingRate->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "SMI iViewNG Connector", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "Load Configuration", 0, QApplication::UnicodeUTF8));
        actionSave_Configuration->setText(QApplication::translate("MainWindow", "Save Configuration", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit_2->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Connection Setup", 0, QApplication::UnicodeUTF8));
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
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Eye Tracking and Eye Video Access", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Eye Camera Resolution", 0, QApplication::UnicodeUTF8));
        eyeResolution->clear();
        eyeResolution->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "320x240", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "640x480", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "current setting", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        eyeResolution->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label_10->setText(QApplication::translate("MainWindow", "Eye Camera Sampling Rate", 0, QApplication::UnicodeUTF8));
        eyeSamplingRate->clear();
        eyeSamplingRate->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "0", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "30", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "60", 0, QApplication::UnicodeUTF8)
        );
        label_9->setText(QApplication::translate("MainWindow", "Eye Camera Tracking Side", 0, QApplication::UnicodeUTF8));
        eyeTrackSides->clear();
        eyeTrackSides->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "left eye", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "right eye", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "both eyes", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        eyeTrackSides->setToolTip(QApplication::translate("MainWindow", "Tracking can be set to use only one eye if necessary.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_14->setText(QApplication::translate("MainWindow", "Uncompressed Left Eye Video", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        addLeftEyeVideo->setToolTip(QApplication::translate("MainWindow", "If checked, an uncompressed stream of the left eye camera will be included.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        addLeftEyeVideo->setText(QApplication::translate("MainWindow", "(add)", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("MainWindow", "Uncompressed Right Eye Video", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        addRightEyeVideo->setToolTip(QApplication::translate("MainWindow", "If checked, an uncompressed stream of the right eye camera will be included.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        addRightEyeVideo->setText(QApplication::translate("MainWindow", "(add)", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("MainWindow", "Eye Tracking Stream", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        addGazeStream->setToolTip(QApplication::translate("MainWindow", "If checked, a stream of deduced gaze parameters will be included.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        addGazeStream->setText(QApplication::translate("MainWindow", "(add)", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Scene Video Access", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Scene Camera Sampling Rate", 0, QApplication::UnicodeUTF8));
        sceneSamplingRate->clear();
        sceneSamplingRate->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "0", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "24", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "30", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "60", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "120", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "240", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        sceneSamplingRate->setToolTip(QApplication::translate("MainWindow", "Note that not all sampling rates might be supported by your device. Also, note that higher sampling rates use more bandwidth.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_6->setText(QApplication::translate("MainWindow", "Scene Camera Resolution", 0, QApplication::UnicodeUTF8));
        sceneResolution->clear();
        sceneResolution->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "320x240", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "400x300", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "640x480", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "960x720", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1280x720", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1280x960", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "current setting", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        sceneResolution->setToolTip(QApplication::translate("MainWindow", "Consider using a low resolution when including an uncompressed scene stream", 0, QApplication::UnicodeUTF8));
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
        label_16->setText(QApplication::translate("MainWindow", "Scene Camera White Balance", 0, QApplication::UnicodeUTF8));
        sceneWhiteBalance->clear();
        sceneWhiteBalance->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Automatic", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Outdoor Daylight", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Outdoor Cloudy", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Indoor Incandescent (Tungsten)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Indoor Fluorescent (White)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Indoor Fluorescent (Daylight)", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        sceneWhiteBalance->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label_13->setText(QApplication::translate("MainWindow", "Uncompressed Scene Video", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        addUncompressedSceneStream->setToolTip(QApplication::translate("MainWindow", "If checked, an uncompressed scene video stream will be generated -- do not record this stream to disk, as it is very high bandwidth.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        addUncompressedSceneStream->setText(QApplication::translate("MainWindow", "(add)", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainWindow", "Compressed Scene Video", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        addCompressedSceneStream->setToolTip(QApplication::translate("MainWindow", "If checked, a compressed scene-image stream will be generated that is mostly useful for recording to disk.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        addCompressedSceneStream->setText(QApplication::translate("MainWindow", "(add)", 0, QApplication::UnicodeUTF8));
        linkButton->setText(QApplication::translate("MainWindow", "Link", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
