/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri Jul 20 18:35:41 2012
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
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label;
    QLineEdit *serverAddress;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QComboBox *setupName;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_2;
    QLabel *label_3;
    QPlainTextEdit *markerNames;
    QPlainTextEdit *rigidNames;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QCheckBox *rigidStreams;
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
        MainWindow->resize(517, 365);
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
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);

        serverAddress = new QLineEdit(centralWidget);
        serverAddress->setObjectName(QString::fromUtf8("serverAddress"));
        serverAddress->setMinimumSize(QSize(106, 0));

        horizontalLayout_2->addWidget(serverAddress);


        verticalLayout->addLayout(horizontalLayout_2);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_4->addWidget(label_4);

        setupName = new QComboBox(groupBox);
        setupName->setObjectName(QString::fromUtf8("setupName"));
        setupName->setMinimumSize(QSize(111, 0));
        setupName->setEditable(true);

        horizontalLayout_4->addWidget(setupName);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        gridLayout->addLayout(horizontalLayout_4, 0, 0, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 1, 1, 1, 1);

        markerNames = new QPlainTextEdit(groupBox);
        markerNames->setObjectName(QString::fromUtf8("markerNames"));

        gridLayout->addWidget(markerNames, 2, 0, 1, 1);

        rigidNames = new QPlainTextEdit(groupBox);
        rigidNames->setObjectName(QString::fromUtf8("rigidNames"));

        gridLayout->addWidget(rigidNames, 2, 1, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer_3 = new QSpacerItem(108, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        rigidStreams = new QCheckBox(groupBox);
        rigidStreams->setObjectName(QString::fromUtf8("rigidStreams"));

        horizontalLayout_3->addWidget(rigidStreams);


        gridLayout->addLayout(horizontalLayout_3, 3, 1, 1, 1);

        label_2->raise();
        label_3->raise();
        markerNames->raise();
        rigidNames->raise();
        rigidStreams->raise();
        rigidStreams->raise();
        label_4->raise();
        setupName->raise();

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

        MainWindow->setCentralWidget(centralWidget);
        serverAddress->raise();
        label->raise();
        groupBox->raise();
        linkButton->raise();
        linkButton->raise();
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 517, 18));
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
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "PhaseSpace Connector", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "&Load Configuration...", 0, QApplication::UnicodeUTF8));
        action_Save_Configuration->setText(QApplication::translate("MainWindow", "&Save Configuration...", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "PhaseSpace Server Address", 0, QApplication::UnicodeUTF8));
        serverAddress->setText(QApplication::translate("MainWindow", "192.168.1.230", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Object Descriptions", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Name of the Setup", 0, QApplication::UnicodeUTF8));
        setupName->clear();
        setupName->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "untitled", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        setupName->setToolTip(QApplication::translate("MainWindow", "Allows to assign a name for the physical setup; preferably same as the used setup file on the server/master.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("MainWindow", "Names for Markers", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Names for Rigid Bodies", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        markerNames->setToolTip(QApplication::translate("MainWindow", "Lets you name your markers. Optional - additional markers provided by the system will be named MarkerN (N corresponding to the marker ID).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        rigidNames->setToolTip(QApplication::translate("MainWindow", "Lets you name your rigid bodies. Optional - additional rigid bodies provided by the system will be named RigidN (N corresponding to the rigid body ID).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        rigidStreams->setToolTip(QApplication::translate("MainWindow", "Spawn an extra stream per rigid body -- practical for real-time applications that care only about a specific object rather than everything that's being mo-capped.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        rigidStreams->setText(QApplication::translate("MainWindow", "One stream per rigid body", 0, QApplication::UnicodeUTF8));
        linkButton->setText(QApplication::translate("MainWindow", "Link", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
