/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Wed Feb 3 14:54:33 2016
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
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *actionQuit_2;
    QAction *actionLoad_Configuration;
    QAction *actionSave_Configuration;
    QAction *actionQuit;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_3;
    QComboBox *samplingRate;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *linkButton;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QString::fromUtf8("MainWindowClass"));
        MainWindowClass->resize(342, 145);
        actionQuit_2 = new QAction(MainWindowClass);
        actionQuit_2->setObjectName(QString::fromUtf8("actionQuit_2"));
        actionLoad_Configuration = new QAction(MainWindowClass);
        actionLoad_Configuration->setObjectName(QString::fromUtf8("actionLoad_Configuration"));
        actionSave_Configuration = new QAction(MainWindowClass);
        actionSave_Configuration->setObjectName(QString::fromUtf8("actionSave_Configuration"));
        actionQuit = new QAction(MainWindowClass);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        formLayout = new QFormLayout(groupBox);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        samplingRate = new QComboBox(groupBox);
        samplingRate->setObjectName(QString::fromUtf8("samplingRate"));

        formLayout->setWidget(0, QFormLayout::FieldRole, samplingRate);


        verticalLayout->addWidget(groupBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        linkButton = new QPushButton(centralWidget);
        linkButton->setObjectName(QString::fromUtf8("linkButton"));

        horizontalLayout->addWidget(linkButton);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);

        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 342, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindowClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionLoad_Configuration);
        menuFile->addAction(actionSave_Configuration);
        menuFile->addAction(actionQuit);

        retranslateUi(MainWindowClass);

        samplingRate->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "eegApp", 0, QApplication::UnicodeUTF8));
        actionQuit_2->setText(QApplication::translate("MainWindowClass", "Quit", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindowClass", "Load Configuration", 0, QApplication::UnicodeUTF8));
        actionSave_Configuration->setText(QApplication::translate("MainWindowClass", "Save Configuration", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindowClass", "Quit", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindowClass", "Device Settings", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindowClass", "Sampling Rate", 0, QApplication::UnicodeUTF8));
        samplingRate->clear();
        samplingRate->insertItems(0, QStringList()
         << QApplication::translate("MainWindowClass", "500", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindowClass", "512", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindowClass", "1000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindowClass", "1024", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindowClass", "2000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindowClass", "2048", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindowClass", "4000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindowClass", "4960", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindowClass", "8000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindowClass", "8192", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindowClass", "16000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindowClass", "16384", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        samplingRate->setToolTip(QApplication::translate("MainWindowClass", "Sampling rate to use.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        linkButton->setText(QApplication::translate("MainWindowClass", "Link", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindowClass", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
