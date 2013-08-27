/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue Aug 27 05:31:23 2013
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
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_4;
    QSpinBox *comPort;
    QLabel *label_3;
    QSpinBox *baudRate;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout_2;
    QLabel *label;
    QSpinBox *samplingRate;
    QLabel *label_2;
    QLineEdit *streamName;
    QLabel *label_9;
    QSpinBox *chunkSize;
    QHBoxLayout *horizontalLayout_3;
    QGroupBox *groupBox_3;
    QFormLayout *formLayout_3;
    QLabel *label_5;
    QComboBox *dataBits;
    QLabel *label_6;
    QComboBox *parity;
    QLabel *label_7;
    QComboBox *stopBits;
    QGroupBox *groupBox_4;
    QFormLayout *formLayout_4;
    QLabel *label_10;
    QSpinBox *readIntervalTimeout;
    QLabel *label_11;
    QSpinBox *readTotalTimeoutConstant;
    QLabel *label_12;
    QSpinBox *readTotalTimeoutMultiplier;
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
        MainWindow->setEnabled(true);
        MainWindow->resize(415, 333);
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
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
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

        comPort = new QSpinBox(groupBox);
        comPort->setObjectName(QString::fromUtf8("comPort"));
        comPort->setMinimum(1);
        comPort->setMaximum(256);

        formLayout->setWidget(0, QFormLayout::FieldRole, comPort);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        baudRate = new QSpinBox(groupBox);
        baudRate->setObjectName(QString::fromUtf8("baudRate"));
        baudRate->setMaximum(100000000);
        baudRate->setValue(57600);

        formLayout->setWidget(1, QFormLayout::FieldRole, baudRate);


        horizontalLayout_2->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        formLayout_2 = new QFormLayout(groupBox_2);
        formLayout_2->setSpacing(6);
        formLayout_2->setContentsMargins(11, 11, 11, 11);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label);

        samplingRate = new QSpinBox(groupBox_2);
        samplingRate->setObjectName(QString::fromUtf8("samplingRate"));
        samplingRate->setMinimum(0);
        samplingRate->setMaximum(1000);
        samplingRate->setValue(0);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, samplingRate);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_2);

        streamName = new QLineEdit(groupBox_2);
        streamName->setObjectName(QString::fromUtf8("streamName"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, streamName);

        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_9);

        chunkSize = new QSpinBox(groupBox_2);
        chunkSize->setObjectName(QString::fromUtf8("chunkSize"));
        chunkSize->setMinimum(1);
        chunkSize->setMaximum(256000);
        chunkSize->setValue(32);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, chunkSize);


        horizontalLayout_2->addWidget(groupBox_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        groupBox_3 = new QGroupBox(centralWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        formLayout_3 = new QFormLayout(groupBox_3);
        formLayout_3->setSpacing(6);
        formLayout_3->setContentsMargins(11, 11, 11, 11);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_5);

        dataBits = new QComboBox(groupBox_3);
        dataBits->setObjectName(QString::fromUtf8("dataBits"));
        dataBits->setEditable(false);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, dataBits);

        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_6);

        parity = new QComboBox(groupBox_3);
        parity->setObjectName(QString::fromUtf8("parity"));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, parity);

        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout_3->setWidget(2, QFormLayout::LabelRole, label_7);

        stopBits = new QComboBox(groupBox_3);
        stopBits->setObjectName(QString::fromUtf8("stopBits"));

        formLayout_3->setWidget(2, QFormLayout::FieldRole, stopBits);


        horizontalLayout_3->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(centralWidget);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        formLayout_4 = new QFormLayout(groupBox_4);
        formLayout_4->setSpacing(6);
        formLayout_4->setContentsMargins(11, 11, 11, 11);
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, label_10);

        readIntervalTimeout = new QSpinBox(groupBox_4);
        readIntervalTimeout->setObjectName(QString::fromUtf8("readIntervalTimeout"));
        readIntervalTimeout->setMaximum(10000000);
        readIntervalTimeout->setValue(500);

        formLayout_4->setWidget(0, QFormLayout::FieldRole, readIntervalTimeout);

        label_11 = new QLabel(groupBox_4);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout_4->setWidget(1, QFormLayout::LabelRole, label_11);

        readTotalTimeoutConstant = new QSpinBox(groupBox_4);
        readTotalTimeoutConstant->setObjectName(QString::fromUtf8("readTotalTimeoutConstant"));
        readTotalTimeoutConstant->setMaximum(10000000);
        readTotalTimeoutConstant->setValue(50);

        formLayout_4->setWidget(1, QFormLayout::FieldRole, readTotalTimeoutConstant);

        label_12 = new QLabel(groupBox_4);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        formLayout_4->setWidget(2, QFormLayout::LabelRole, label_12);

        readTotalTimeoutMultiplier = new QSpinBox(groupBox_4);
        readTotalTimeoutMultiplier->setObjectName(QString::fromUtf8("readTotalTimeoutMultiplier"));
        readTotalTimeoutMultiplier->setMaximum(1000000);
        readTotalTimeoutMultiplier->setValue(10);

        formLayout_4->setWidget(2, QFormLayout::FieldRole, readTotalTimeoutMultiplier);


        horizontalLayout_3->addWidget(groupBox_4);


        verticalLayout->addLayout(horizontalLayout_3);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

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
        menuBar->setGeometry(QRect(0, 0, 415, 18));
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

        dataBits->setCurrentIndex(4);
        parity->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Serial Port Connector", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "Load Configuration", 0, QApplication::UnicodeUTF8));
        actionSave_Configuration->setText(QApplication::translate("MainWindow", "Save Configuration", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit_2->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Core Port Settings", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "COM Port", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        comPort->setToolTip(QApplication::translate("MainWindow", "This is the COM port number; these are usually consecutively assigned to devices.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("MainWindow", "Baud Rate", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        baudRate->setToolTip(QApplication::translate("MainWindow", "This is in bits per second.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Stream Settings", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Stream Sampling Rate", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        samplingRate->setToolTip(QApplication::translate("MainWindow", "Set this to a non-zero value if you know the sampling rate.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("MainWindow", "Stream Name", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        streamName->setToolTip(QApplication::translate("MainWindow", "The name of the stream within LSL.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        streamName->setText(QApplication::translate("MainWindow", "SerialPort", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "Chunk Size", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        chunkSize->setToolTip(QApplication::translate("MainWindow", "The number of samples (=bytes) that are grouped into a chunk for transmission into LSL.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Misc Port Settings", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Data Bits", 0, QApplication::UnicodeUTF8));
        dataBits->clear();
        dataBits->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "4", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "6", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "7", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "8", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        dataBits->setToolTip(QApplication::translate("MainWindow", "Number of data bits per byte", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_6->setText(QApplication::translate("MainWindow", "Parity", 0, QApplication::UnicodeUTF8));
        parity->clear();
        parity->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "None", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Odd", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Even", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Mark", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Space", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        parity->setToolTip(QApplication::translate("MainWindow", "Parity checking scheme.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_7->setText(QApplication::translate("MainWindow", "Stop Bits", 0, QApplication::UnicodeUTF8));
        stopBits->clear();
        stopBits->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1.5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "2", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        stopBits->setToolTip(QApplication::translate("MainWindow", "Number of stop bits.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        groupBox_4->setToolTip(QApplication::translate("MainWindow", "The timeout settings are largely irrelevant in this simple context but are exposed for completeness.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Misc Timeout Settings (ms)", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainWindow", "Read Interval Timeout", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        readIntervalTimeout->setToolTip(QApplication::translate("MainWindow", "Maximum time that may pass between two bytes ", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_11->setText(QApplication::translate("MainWindow", "Read Total Timeout Constant", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        readTotalTimeoutConstant->setToolTip(QApplication::translate("MainWindow", "Constant term in length-dependent timeout.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_12->setText(QApplication::translate("MainWindow", "Read Total Timeout Multiplier", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        readTotalTimeoutMultiplier->setToolTip(QApplication::translate("MainWindow", "Linear term in length-dependent timeout.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        linkButton->setText(QApplication::translate("MainWindow", "Link", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
