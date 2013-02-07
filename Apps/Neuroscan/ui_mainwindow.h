/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sun Jul 29 22:40:23 2012
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
    QLabel *label_3;
    QComboBox *ampType;
    QLabel *label_4;
    QLineEdit *wirelessAddress;
    QLabel *label;
    QComboBox *samplingRate;
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
        MainWindow->resize(236, 200);
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
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        ampType = new QComboBox(centralWidget);
        ampType->setObjectName(QString::fromUtf8("ampType"));

        formLayout->setWidget(0, QFormLayout::FieldRole, ampType);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_4);

        wirelessAddress = new QLineEdit(centralWidget);
        wirelessAddress->setObjectName(QString::fromUtf8("wirelessAddress"));

        formLayout->setWidget(1, QFormLayout::FieldRole, wirelessAddress);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label);

        samplingRate = new QComboBox(centralWidget);
        samplingRate->setObjectName(QString::fromUtf8("samplingRate"));

        formLayout->setWidget(2, QFormLayout::FieldRole, samplingRate);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_2);

        channelCount = new QSpinBox(centralWidget);
        channelCount->setObjectName(QString::fromUtf8("channelCount"));
        channelCount->setMinimum(1);
        channelCount->setMaximum(70);
        channelCount->setValue(64);

        formLayout->setWidget(3, QFormLayout::FieldRole, channelCount);


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
        menuBar->setGeometry(QRect(0, 0, 236, 18));
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
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Neuroscan Connector", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "Load Configuration", 0, QApplication::UnicodeUTF8));
        actionSave_Configuration->setText(QApplication::translate("MainWindow", "Save Configuration", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit_2->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Amplifier Type", 0, QApplication::UnicodeUTF8));
        ampType->clear();
        ampType->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Synamp II", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Synamp Wireless", 0, QApplication::UnicodeUTF8)
        );
        label_4->setText(QApplication::translate("MainWindow", "Wireless Address", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Sampling Rate", 0, QApplication::UnicodeUTF8));
        samplingRate->clear();
        samplingRate->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "100", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "200", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "250", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "500", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1250", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "2000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "2500", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "5000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "10000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "20000", 0, QApplication::UnicodeUTF8)
        );
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
