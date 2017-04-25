/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
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
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
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
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QPlainTextEdit *channelLabels;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_4;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_4;
    QLabel *label_2;
    QSpinBox *channelCount;
    QLabel *label;
    QSpinBox *chunkSize;
    QLabel *label_3;
    QComboBox *samplingRate;
    QLabel *label_7;
    QCheckBox *useAUX;
    QLabel *label_8;
    QCheckBox *useACC;
    QLabel *label_12;
    QCheckBox *useBipolar;
    QLineEdit *deviceSerialNumber;
    QPushButton *linkButton;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox_3;
    QFormLayout *formLayout_2;
    QCheckBox *unsampledMarkers;
    QLabel *label_6;
    QCheckBox *sampledMarkers;
    QLabel *label_9;
    QCheckBox *sampledMarkersEEG;
    QLabel *label_5;
    QGroupBox *groupBox_4;
    QFormLayout *formLayout_3;
    QPushButton *refreshDevices;
    QLabel *label_10;
    QComboBox *deviceCb;
    QLabel *label_11;
    QCheckBox *useSim;
    QHBoxLayout *horizontalLayout;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(409, 633);
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
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        channelLabels = new QPlainTextEdit(groupBox_2);
        channelLabels->setObjectName(QString::fromUtf8("channelLabels"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(channelLabels->sizePolicy().hasHeightForWidth());
        channelLabels->setSizePolicy(sizePolicy);

        gridLayout->addWidget(channelLabels, 0, 0, 1, 1);


        horizontalLayout_2->addWidget(groupBox_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));

        verticalLayout->addLayout(horizontalLayout_4);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        formLayout = new QFormLayout(groupBox);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_2);

        channelCount = new QSpinBox(groupBox);
        channelCount->setObjectName(QString::fromUtf8("channelCount"));
        channelCount->setMinimum(1);
        channelCount->setMaximum(256);
        channelCount->setSingleStep(1);
        channelCount->setValue(32);

        formLayout->setWidget(3, QFormLayout::FieldRole, channelCount);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label);

        chunkSize = new QSpinBox(groupBox);
        chunkSize->setObjectName(QString::fromUtf8("chunkSize"));
        chunkSize->setMinimum(1);
        chunkSize->setMaximum(20);
        chunkSize->setValue(10);

        formLayout->setWidget(4, QFormLayout::FieldRole, chunkSize);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_3);

        samplingRate = new QComboBox(groupBox);
        samplingRate->setObjectName(QString::fromUtf8("samplingRate"));

        formLayout->setWidget(5, QFormLayout::FieldRole, samplingRate);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_7);

        useAUX = new QCheckBox(groupBox);
        useAUX->setObjectName(QString::fromUtf8("useAUX"));
        useAUX->setChecked(false);

        formLayout->setWidget(6, QFormLayout::FieldRole, useAUX);

        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_8);

        useACC = new QCheckBox(groupBox);
        useACC->setObjectName(QString::fromUtf8("useACC"));
        useACC->setChecked(true);

        formLayout->setWidget(7, QFormLayout::FieldRole, useACC);

        label_12 = new QLabel(groupBox);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        formLayout->setWidget(8, QFormLayout::LabelRole, label_12);

        useBipolar = new QCheckBox(groupBox);
        useBipolar->setObjectName(QString::fromUtf8("useBipolar"));

        formLayout->setWidget(8, QFormLayout::FieldRole, useBipolar);

        deviceSerialNumber = new QLineEdit(groupBox);
        deviceSerialNumber->setObjectName(QString::fromUtf8("deviceSerialNumber"));

        formLayout->setWidget(2, QFormLayout::LabelRole, deviceSerialNumber);


        verticalLayout->addWidget(groupBox);

        linkButton = new QPushButton(centralWidget);
        linkButton->setObjectName(QString::fromUtf8("linkButton"));
        linkButton->setEnabled(true);

        verticalLayout->addWidget(linkButton, 0, Qt::AlignRight);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        groupBox_3 = new QGroupBox(centralWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        formLayout_2 = new QFormLayout(groupBox_3);
        formLayout_2->setSpacing(6);
        formLayout_2->setContentsMargins(11, 11, 11, 11);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        unsampledMarkers = new QCheckBox(groupBox_3);
        unsampledMarkers->setObjectName(QString::fromUtf8("unsampledMarkers"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, unsampledMarkers);

        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_6);

        sampledMarkers = new QCheckBox(groupBox_3);
        sampledMarkers->setObjectName(QString::fromUtf8("sampledMarkers"));
        sampledMarkers->setChecked(true);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, sampledMarkers);

        label_9 = new QLabel(groupBox_3);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_9);

        sampledMarkersEEG = new QCheckBox(groupBox_3);
        sampledMarkersEEG->setObjectName(QString::fromUtf8("sampledMarkersEEG"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, sampledMarkersEEG);

        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_5);


        verticalLayout->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(centralWidget);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        formLayout_3 = new QFormLayout(groupBox_4);
        formLayout_3->setSpacing(6);
        formLayout_3->setContentsMargins(11, 11, 11, 11);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        refreshDevices = new QPushButton(groupBox_4);
        refreshDevices->setObjectName(QString::fromUtf8("refreshDevices"));
        refreshDevices->setEnabled(true);

        formLayout_3->setWidget(3, QFormLayout::SpanningRole, refreshDevices);

        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_3->setWidget(4, QFormLayout::LabelRole, label_10);

        deviceCb = new QComboBox(groupBox_4);
        deviceCb->setObjectName(QString::fromUtf8("deviceCb"));

        formLayout_3->setWidget(4, QFormLayout::FieldRole, deviceCb);

        label_11 = new QLabel(groupBox_4);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout_3->setWidget(5, QFormLayout::LabelRole, label_11);

        useSim = new QCheckBox(groupBox_4);
        useSim->setObjectName(QString::fromUtf8("useSim"));

        formLayout_3->setWidget(5, QFormLayout::FieldRole, useSim);


        verticalLayout->addWidget(groupBox_4);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 409, 21));
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

        samplingRate->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "LiveAmp Connector", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "Load Configuration", 0, QApplication::UnicodeUTF8));
        actionSave_Configuration->setText(QApplication::translate("MainWindow", "Save Configuration", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit_2->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Channel Labels", 0, QApplication::UnicodeUTF8));
        channelLabels->setPlainText(QApplication::translate("MainWindow", "Fp1\n"
"Fp2\n"
"F7\n"
"F3\n"
"Fz\n"
"F4\n"
"F8\n"
"FC5\n"
"FC1\n"
"FC2\n"
"FC6\n"
"T7\n"
"C3\n"
"Cz\n"
"C4\n"
"T8\n"
"TP9\n"
"CP5\n"
"CP1\n"
"CP2\n"
"CP6\n"
"TP10\n"
"P7\n"
"P3\n"
"Pz\n"
"P4\n"
"P8\n"
"PO9\n"
"O1\n"
"Oz\n"
"O2\n"
"PO10 ", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Device Settings", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Device Number", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Number of EEG channels", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        channelCount->setToolTip(QApplication::translate("MainWindow", "This must match the number of entries in the channel list", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("MainWindow", "Chunk Size", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        chunkSize->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>The number of samples per chunk emitted by the driver -- a small value will lead to lower overall latency but causes more CPU load, max value is 20</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("MainWindow", "Sampling Rate", 0, QApplication::UnicodeUTF8));
        samplingRate->clear();
        samplingRate->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "250", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "500", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1000", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        samplingRate->setToolTip(QApplication::translate("MainWindow", "The sampling rate to use; higher sampling rates require more network bandwidth (and storage space if recording), particularly the very high rates of 10KHz+. The native rates are those that are natively supported by the hardware and the resampled rates are resampled in software  (using a linear-phase sinc resampler that delays the output signal by 5 samples).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_7->setText(QApplication::translate("MainWindow", "Use AUX Channels", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        useAUX->setToolTip(QApplication::translate("MainWindow", "If this is checked then the last 8 channels will hold the AUX signals; make sure to increase your channel count accordingly.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        useAUX->setText(QApplication::translate("MainWindow", "(check)", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Enable ACC sensors", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        useACC->setToolTip(QApplication::translate("MainWindow", "This will enable Acceleration sensors X, Y and Z.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        useACC->setText(QApplication::translate("MainWindow", "(check)", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("MainWindow", "Enable Bipolar (last 8 only) ", 0, QApplication::UnicodeUTF8));
        useBipolar->setText(QApplication::translate("MainWindow", "(check)", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        deviceSerialNumber->setToolTip(QApplication::translate("MainWindow", "The number of the USB device (if multiple); the first one is #0.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        deviceSerialNumber->setInputMask(QString());
        deviceSerialNumber->setText(QString());
        linkButton->setText(QApplication::translate("MainWindow", "Link", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "LSL Trigger Output Style", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        unsampledMarkers->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>For an explanation of trigger marker types please read 'explanation_of_trigger_marker_types.pdf'.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        unsampledMarkers->setText(QApplication::translate("MainWindow", "(check)", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Sampled String Markers", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        sampledMarkers->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>For an explanation of trigger marker types please read 'explanation_of_trigger_marker_types.pdf'.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        sampledMarkers->setText(QApplication::translate("MainWindow", "(check)", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "Include in EEG Stream", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        sampledMarkersEEG->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>For an explanation of trigger marker types please read 'explanation_of_trigger_marker_types.pdf'.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        sampledMarkersEEG->setText(QApplication::translate("MainWindow", "(check)", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Unsampled String Markers", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Scan For Devices", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        refreshDevices->setToolTip(QApplication::translate("MainWindow", "This will scan for all available LiveAmps. It takes a few moments to connect. You need to have the device serial number (written on the back) in order to connect to it. You can also enter it into the Device Number box by hand.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        refreshDevices->setText(QApplication::translate("MainWindow", "Scan", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainWindow", "Available Devices", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainWindow", "Use Simulator", 0, QApplication::UnicodeUTF8));
        useSim->setText(QApplication::translate("MainWindow", "(check)", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
