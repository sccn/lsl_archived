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
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QLineEdit *le_ts;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *le_ec;
    QPushButton *click;
    QPushButton *start;
    QPushButton *stop;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(250, 245);
        MainWindow->setMinimumSize(QSize(250, 200));
        MainWindow->setMaximumSize(QSize(250, 525));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        le_ts = new QLineEdit(centralWidget);
        le_ts->setObjectName(QString::fromUtf8("le_ts"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(le_ts->sizePolicy().hasHeightForWidth());
        le_ts->setSizePolicy(sizePolicy);
        le_ts->setReadOnly(true);

        gridLayout->addWidget(le_ts, 6, 1, 1, 1);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 6, 0, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 7, 0, 1, 1);

        le_ec = new QLineEdit(centralWidget);
        le_ec->setObjectName(QString::fromUtf8("le_ec"));
        le_ec->setReadOnly(true);

        gridLayout->addWidget(le_ec, 7, 1, 1, 1);

        click = new QPushButton(centralWidget);
        click->setObjectName(QString::fromUtf8("click"));
        click->setEnabled(false);
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(click->sizePolicy().hasHeightForWidth());
        click->setSizePolicy(sizePolicy1);
        click->setMinimumSize(QSize(0, 100));

        gridLayout->addWidget(click, 2, 0, 1, 2);

        start = new QPushButton(centralWidget);
        start->setObjectName(QString::fromUtf8("start"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(start->sizePolicy().hasHeightForWidth());
        start->setSizePolicy(sizePolicy2);
        start->setMinimumSize(QSize(25, 0));

        gridLayout->addWidget(start, 0, 0, 1, 1);

        stop = new QPushButton(centralWidget);
        stop->setObjectName(QString::fromUtf8("stop"));
        stop->setEnabled(false);
        sizePolicy2.setHeightForWidth(stop->sizePolicy().hasHeightForWidth());
        stop->setSizePolicy(sizePolicy2);
        stop->setMinimumSize(QSize(43, 0));

        gridLayout->addWidget(stop, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 250, 18));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "ClickMe", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Time Stamp", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Event Code", 0, QApplication::UnicodeUTF8));
        click->setText(QApplication::translate("MainWindow", "Click Me!", 0, QApplication::UnicodeUTF8));
        start->setText(QApplication::translate("MainWindow", "Start Stream", 0, QApplication::UnicodeUTF8));
        stop->setText(QApplication::translate("MainWindow", "Stop Stream", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
