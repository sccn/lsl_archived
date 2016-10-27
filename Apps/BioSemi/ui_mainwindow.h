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
#include <QtGui/QGridLayout>
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
    QAction *action_Save_Configuration;
    QAction *actionQuit;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *referenceChannels;
    QLabel *label_2;
    QLineEdit *capLocation;
    QPushButton *browseLocation;
    QLabel *label_3;
    QComboBox *capDesign;
    QLabel *label_6;
    QComboBox *channelSubset;
    QLabel *label_4;
    QSpinBox *capCircumference;
    QLabel *label_5;
    QCheckBox *resamplingOn;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *connectButton;
    QMenuBar *menuBar;
    QMenu *menuFilw;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(339, 292);
        actionLoad_Configuration = new QAction(MainWindow);
        actionLoad_Configuration->setObjectName(QString::fromUtf8("actionLoad_Configuration"));
        action_Save_Configuration = new QAction(MainWindow);
        action_Save_Configuration->setObjectName(QString::fromUtf8("action_Save_Configuration"));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        referenceChannels = new QLineEdit(groupBox);
        referenceChannels->setObjectName(QString::fromUtf8("referenceChannels"));

        gridLayout->addWidget(referenceChannels, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        capLocation = new QLineEdit(groupBox);
        capLocation->setObjectName(QString::fromUtf8("capLocation"));

        gridLayout->addWidget(capLocation, 1, 1, 1, 1);

        browseLocation = new QPushButton(groupBox);
        browseLocation->setObjectName(QString::fromUtf8("browseLocation"));

        gridLayout->addWidget(browseLocation, 1, 2, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        capDesign = new QComboBox(groupBox);
        capDesign->setObjectName(QString::fromUtf8("capDesign"));
        capDesign->setEditable(true);

        gridLayout->addWidget(capDesign, 2, 1, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 3, 0, 1, 1);

        channelSubset = new QComboBox(groupBox);
        channelSubset->setObjectName(QString::fromUtf8("channelSubset"));

        gridLayout->addWidget(channelSubset, 3, 1, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        capCircumference = new QSpinBox(groupBox);
        capCircumference->setObjectName(QString::fromUtf8("capCircumference"));
        capCircumference->setMinimum(40);
        capCircumference->setMaximum(70);
        capCircumference->setValue(54);

        gridLayout->addWidget(capCircumference, 4, 1, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 5, 0, 1, 1);

        resamplingOn = new QCheckBox(groupBox);
        resamplingOn->setObjectName(QString::fromUtf8("resamplingOn"));

        gridLayout->addWidget(resamplingOn, 5, 1, 1, 1);


        verticalLayout->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        connectButton = new QPushButton(centralWidget);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));

        horizontalLayout->addWidget(connectButton);


        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralWidget);
        groupBox->raise();
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 339, 21));
        menuFilw = new QMenu(menuBar);
        menuFilw->setObjectName(QString::fromUtf8("menuFilw"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFilw->menuAction());
        menuFilw->addAction(actionLoad_Configuration);
        menuFilw->addAction(action_Save_Configuration);
        menuFilw->addSeparator();
        menuFilw->addAction(actionQuit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "BioSemi Device", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "&Load Configuration...", 0, QApplication::UnicodeUTF8));
        action_Save_Configuration->setText(QApplication::translate("MainWindow", "&Save Configuration...", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Experiment Information", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Reference Channels", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        referenceChannels->setToolTip(QApplication::translate("MainWindow", "It is very important to list the correct reference channels here, if any (since BioSemi has no dedicated reference).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        referenceChannels->setText(QApplication::translate("MainWindow", "EX1, EX2", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Location File", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        capLocation->setToolTip(QApplication::translate("MainWindow", "If this is a name of an .sfp location file (e.g. from Zebris), the channel locations will be included in the recording. Make sure that the labels correspond the BioSemi channel names (A1-A32, B1-B32, ... ,EX1-EX8, AUX1-AUX16, AIB1-AIB32) and ideally some standard fiducials.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        browseLocation->setText(QApplication::translate("MainWindow", "&Browse...", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Cap Design", 0, QApplication::UnicodeUTF8));
        capDesign->clear();
        capDesign->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Rita-MoBI-2010-07", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        capDesign->setToolTip(QApplication::translate("MainWindow", "If you are working with multiple cap layouts, it is very important to use systematic names for them and take note of which was used.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_6->setText(QApplication::translate("MainWindow", "EEG Channel Subset", 0, QApplication::UnicodeUTF8));
        channelSubset->clear();
        channelSubset->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "all", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "160", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "128", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "64", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "32", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "all, no AUX", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "160, no AUX", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "128, no AUX", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "64, no AUX", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "32, no AUX", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        channelSubset->setToolTip(QApplication::translate("MainWindow", "Record only a subset of EEG channels (note: this is per box in daisy-chained setups). EX channels will always be recorded.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_4->setText(QApplication::translate("MainWindow", "Cap Circumference", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        capCircumference->setToolTip(QApplication::translate("MainWindow", "The cap circumference in centimeters. 54 is average.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_5->setText(QApplication::translate("MainWindow", "Resample to 512 Hz", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        resamplingOn->setToolTip(QApplication::translate("MainWindow", "Doing this saves space and bandwidth but will introduce 10ms of pre/post-rining artifacts in the EEG (and trigger) channel -- not optimal for studies of sharp timing effects.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        resamplingOn->setText(QString());
        connectButton->setText(QApplication::translate("MainWindow", "&Link", 0, QApplication::UnicodeUTF8));
        menuFilw->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
