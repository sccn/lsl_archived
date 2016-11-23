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
    QAction *action_Save_Configuration;
    QAction *action_Quit;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label_3;
    QComboBox *cameraType;
    QLabel *label;
    QLineEdit *serverAddress;
    QLabel *label_2;
    QLineEdit *clientAddress;
    QLabel *label_4;
    QLineEdit *commandPort;
    QLabel *label_5;
    QLineEdit *dataPort;
    QLabel *label_6;
    QLineEdit *multicastAddress;
    QCheckBox *multicastCheck;
    QLineEdit *fpsEdit;
    QLabel *label_7;
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
        MainWindow->resize(297, 370);
        actionLoad_Configuration = new QAction(MainWindow);
        actionLoad_Configuration->setObjectName(QString::fromUtf8("actionLoad_Configuration"));
        action_Save_Configuration = new QAction(MainWindow);
        action_Save_Configuration->setObjectName(QString::fromUtf8("action_Save_Configuration"));
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
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
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        cameraType = new QComboBox(centralWidget);
        cameraType->setObjectName(QString::fromUtf8("cameraType"));
        cameraType->setEditable(true);

        formLayout->setWidget(0, QFormLayout::FieldRole, cameraType);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label);

        serverAddress = new QLineEdit(centralWidget);
        serverAddress->setObjectName(QString::fromUtf8("serverAddress"));
        serverAddress->setMinimumSize(QSize(106, 0));

        formLayout->setWidget(2, QFormLayout::FieldRole, serverAddress);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_2);

        clientAddress = new QLineEdit(centralWidget);
        clientAddress->setObjectName(QString::fromUtf8("clientAddress"));

        formLayout->setWidget(4, QFormLayout::FieldRole, clientAddress);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_4);

        commandPort = new QLineEdit(centralWidget);
        commandPort->setObjectName(QString::fromUtf8("commandPort"));

        formLayout->setWidget(5, QFormLayout::FieldRole, commandPort);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_5);

        dataPort = new QLineEdit(centralWidget);
        dataPort->setObjectName(QString::fromUtf8("dataPort"));
        dataPort->setLayoutDirection(Qt::LeftToRight);

        formLayout->setWidget(6, QFormLayout::FieldRole, dataPort);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(9, QFormLayout::LabelRole, label_6);

        multicastAddress = new QLineEdit(centralWidget);
        multicastAddress->setObjectName(QString::fromUtf8("multicastAddress"));
        multicastAddress->setEnabled(true);
        multicastAddress->setReadOnly(false);

        formLayout->setWidget(9, QFormLayout::FieldRole, multicastAddress);

        multicastCheck = new QCheckBox(centralWidget);
        multicastCheck->setObjectName(QString::fromUtf8("multicastCheck"));

        formLayout->setWidget(8, QFormLayout::LabelRole, multicastCheck);

        fpsEdit = new QLineEdit(centralWidget);
        fpsEdit->setObjectName(QString::fromUtf8("fpsEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, fpsEdit);

        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_7);


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
        menuBar->setGeometry(QRect(0, 0, 297, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionLoad_Configuration);
        menuFile->addAction(action_Save_Configuration);
        menuFile->addSeparator();
        menuFile->addAction(action_Quit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "OptiTrack-v2.9 Connector", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "&Load Configuration...", 0, QApplication::UnicodeUTF8));
        action_Save_Configuration->setText(QApplication::translate("MainWindow", "&Save Configuration...", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Camera Type", 0, QApplication::UnicodeUTF8));
        cameraType->clear();
        cameraType->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "S250e", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Flex 13", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "V100:R2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "V120:Duo", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "V120:Trio", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "V120:SLIM", 0, QApplication::UnicodeUTF8)
        );
        label->setText(QApplication::translate("MainWindow", "OptiTrack Server Address", 0, QApplication::UnicodeUTF8));
        serverAddress->setText(QString());
        label_2->setText(QApplication::translate("MainWindow", "Local Client Address", 0, QApplication::UnicodeUTF8));
        clientAddress->setText(QApplication::translate("MainWindow", "127.0.0.1", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "OptiTrack Command Port", 0, QApplication::UnicodeUTF8));
        commandPort->setText(QApplication::translate("MainWindow", "3131", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "OptiTrack Data Port", 0, QApplication::UnicodeUTF8));
        dataPort->setText(QApplication::translate("MainWindow", "3130", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Multicast Address", 0, QApplication::UnicodeUTF8));
        multicastAddress->setText(QApplication::translate("MainWindow", "(default)", 0, QApplication::UnicodeUTF8));
        multicastCheck->setText(QApplication::translate("MainWindow", "Enable Multicast", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "FPS", 0, QApplication::UnicodeUTF8));
        linkButton->setText(QApplication::translate("MainWindow", "Link", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
