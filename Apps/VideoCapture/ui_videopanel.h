/********************************************************************************
** Form generated from reading UI file 'videopanel.ui'
**
** Created by: Qt User Interface Compiler version 5.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIDEOPANEL_H
#define UI_VIDEOPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcameraviewfinder.h"

QT_BEGIN_NAMESPACE

class Ui_videoPanel
{
public:
    QCameraViewfinder *viewFinder;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QComboBox *cameraBox;
    QCheckBox *monitorCheck;
    QSpacerItem *verticalSpacer;
    QPushButton *recordingSettingsButton;
    QLabel *label_5;
    QLineEdit *requestedFR;
    QLabel *label_6;
    QHBoxLayout *horizontalLayout;
    QLineEdit *outFile;
    QPushButton *browseButton;
    QPushButton *recordButton;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_4;
    QLineEdit *actualFR;
    QLabel *label_3;
    QLineEdit *frameCount;
    QLabel *label_2;
    QLineEdit *timeStamp;
    QLineEdit *resolutionEdit;
    QLabel *label_7;

    void setupUi(QWidget *videoPanel)
    {
        if (videoPanel->objectName().isEmpty())
            videoPanel->setObjectName(QStringLiteral("videoPanel"));
        videoPanel->resize(731, 502);
        viewFinder = new QCameraViewfinder(videoPanel);
        viewFinder->setObjectName(QStringLiteral("viewFinder"));
        viewFinder->setEnabled(true);
        viewFinder->setGeometry(QRect(230, 40, 491, 421));
        viewFinder->setAutoFillBackground(true);
        verticalLayoutWidget = new QWidget(videoPanel);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 211, 421));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        cameraBox = new QComboBox(verticalLayoutWidget);
        cameraBox->setObjectName(QStringLiteral("cameraBox"));

        verticalLayout->addWidget(cameraBox);

        monitorCheck = new QCheckBox(verticalLayoutWidget);
        monitorCheck->setObjectName(QStringLiteral("monitorCheck"));

        verticalLayout->addWidget(monitorCheck);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        recordingSettingsButton = new QPushButton(verticalLayoutWidget);
        recordingSettingsButton->setObjectName(QStringLiteral("recordingSettingsButton"));
        recordingSettingsButton->setEnabled(false);

        verticalLayout->addWidget(recordingSettingsButton);

        label_5 = new QLabel(verticalLayoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout->addWidget(label_5);

        requestedFR = new QLineEdit(verticalLayoutWidget);
        requestedFR->setObjectName(QStringLiteral("requestedFR"));
        requestedFR->setEnabled(true);
        requestedFR->setReadOnly(true);

        verticalLayout->addWidget(requestedFR);

        label_6 = new QLabel(verticalLayoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        verticalLayout->addWidget(label_6);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        outFile = new QLineEdit(verticalLayoutWidget);
        outFile->setObjectName(QStringLiteral("outFile"));

        horizontalLayout->addWidget(outFile);

        browseButton = new QPushButton(verticalLayoutWidget);
        browseButton->setObjectName(QStringLiteral("browseButton"));

        horizontalLayout->addWidget(browseButton);


        verticalLayout->addLayout(horizontalLayout);

        recordButton = new QPushButton(verticalLayoutWidget);
        recordButton->setObjectName(QStringLiteral("recordButton"));
        recordButton->setEnabled(false);

        verticalLayout->addWidget(recordButton);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        label_4 = new QLabel(verticalLayoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout->addWidget(label_4);

        actualFR = new QLineEdit(verticalLayoutWidget);
        actualFR->setObjectName(QStringLiteral("actualFR"));
        actualFR->setEnabled(true);
        actualFR->setReadOnly(true);

        verticalLayout->addWidget(actualFR);

        label_3 = new QLabel(verticalLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout->addWidget(label_3);

        frameCount = new QLineEdit(verticalLayoutWidget);
        frameCount->setObjectName(QStringLiteral("frameCount"));
        frameCount->setEnabled(true);
        frameCount->setReadOnly(true);

        verticalLayout->addWidget(frameCount);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout->addWidget(label_2);

        timeStamp = new QLineEdit(verticalLayoutWidget);
        timeStamp->setObjectName(QStringLiteral("timeStamp"));
        timeStamp->setEnabled(true);
        timeStamp->setReadOnly(true);

        verticalLayout->addWidget(timeStamp);

        resolutionEdit = new QLineEdit(videoPanel);
        resolutionEdit->setObjectName(QStringLiteral("resolutionEdit"));
        resolutionEdit->setGeometry(QRect(300, 10, 113, 20));
        resolutionEdit->setReadOnly(true);
        label_7 = new QLabel(videoPanel);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(240, 10, 51, 16));

        retranslateUi(videoPanel);

        QMetaObject::connectSlotsByName(videoPanel);
    } // setupUi

    void retranslateUi(QWidget *videoPanel)
    {
        videoPanel->setWindowTitle(QApplication::translate("videoPanel", "Form", 0));
        label->setText(QApplication::translate("videoPanel", "Camera", 0));
        monitorCheck->setText(QApplication::translate("videoPanel", "View", 0));
        recordingSettingsButton->setText(QApplication::translate("videoPanel", "Recording Settings", 0));
        label_5->setText(QApplication::translate("videoPanel", "Requested Frame Rate", 0));
        label_6->setText(QApplication::translate("videoPanel", "Output File", 0));
        browseButton->setText(QApplication::translate("videoPanel", "Browse", 0));
        recordButton->setText(QApplication::translate("videoPanel", "Record", 0));
        label_4->setText(QApplication::translate("videoPanel", "Actual Frame Rate", 0));
        label_3->setText(QApplication::translate("videoPanel", "Frame Count", 0));
        label_2->setText(QApplication::translate("videoPanel", "Timestamp", 0));
        label_7->setText(QApplication::translate("videoPanel", "Resolution", 0));
    } // retranslateUi

};

namespace Ui {
    class videoPanel: public Ui_videoPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIDEOPANEL_H
