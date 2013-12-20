/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Dec 19 17:45:59 2013
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
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_4;
    QLineEdit *deviceNumber;
    QLabel *label_2;
    QSpinBox *channelCount;
    QLabel *label;
    QSpinBox *chunkSize;
    QLabel *label_3;
    QComboBox *samplingRate;
    QCheckBox *isSlave;
    QLabel *label_7;
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
        MainWindow->resize(303, 388);
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

        gridLayout->addWidget(channelLabels, 0, 0, 1, 1);


        horizontalLayout_2->addWidget(groupBox_2);

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
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        deviceNumber = new QLineEdit(groupBox);
        deviceNumber->setObjectName(QString::fromUtf8("deviceNumber"));

        formLayout->setWidget(0, QFormLayout::FieldRole, deviceNumber);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        channelCount = new QSpinBox(groupBox);
        channelCount->setObjectName(QString::fromUtf8("channelCount"));
        channelCount->setMinimum(1);
        channelCount->setMaximum(256);
        channelCount->setValue(256);

        formLayout->setWidget(1, QFormLayout::FieldRole, channelCount);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label);

        chunkSize = new QSpinBox(groupBox);
        chunkSize->setObjectName(QString::fromUtf8("chunkSize"));
        chunkSize->setMinimum(1);
        chunkSize->setMaximum(256);
        chunkSize->setValue(32);

        formLayout->setWidget(2, QFormLayout::FieldRole, chunkSize);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        samplingRate = new QComboBox(groupBox);
        samplingRate->setObjectName(QString::fromUtf8("samplingRate"));

        formLayout->setWidget(3, QFormLayout::FieldRole, samplingRate);

        isSlave = new QCheckBox(groupBox);
        isSlave->setObjectName(QString::fromUtf8("isSlave"));

        formLayout->setWidget(5, QFormLayout::FieldRole, isSlave);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_7);


        verticalLayout->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

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


        horizontalLayout_2->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 303, 18));
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

        samplingRate->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "gHIamp Connector", 0, QApplication::UnicodeUTF8));
        actionLoad_Configuration->setText(QApplication::translate("MainWindow", "Load Configuration", 0, QApplication::UnicodeUTF8));
        actionSave_Configuration->setText(QApplication::translate("MainWindow", "Save Configuration", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit_2->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Channel Labels", 0, QApplication::UnicodeUTF8));
        channelLabels->setPlainText(QApplication::translate("MainWindow", "Ch1\n"
"Ch2\n"
"Ch3\n"
"Ch4\n"
"Ch5\n"
"Ch6\n"
"Ch7\n"
"Ch8\n"
"Ch9\n"
"Ch10\n"
"Ch11\n"
"Ch12\n"
"Ch13\n"
"Ch14\n"
"Ch15\n"
"Ch16\n"
"Ch17\n"
"Ch18\n"
"Ch19\n"
"Ch20\n"
"Ch21\n"
"Ch22\n"
"Ch23\n"
"Ch24\n"
"Ch25\n"
"Ch26\n"
"Ch27\n"
"Ch28\n"
"Ch29\n"
"Ch30\n"
"Ch31\n"
"Ch32\n"
"Ch33\n"
"Ch34\n"
"Ch35\n"
"Ch36\n"
"Ch37\n"
"Ch38\n"
"Ch39\n"
"Ch40\n"
"Ch41\n"
"Ch42\n"
"Ch43\n"
"Ch44\n"
"Ch45\n"
"Ch46\n"
"Ch47\n"
"Ch48\n"
"Ch49\n"
"Ch50\n"
"Ch51\n"
"Ch52\n"
"Ch53\n"
"Ch54\n"
"Ch55\n"
"Ch56\n"
"Ch57\n"
"Ch58\n"
"Ch59\n"
"Ch60\n"
"Ch61\n"
"Ch62\n"
"Ch63\n"
"Ch64\n"
"Ch65\n"
"Ch66\n"
"Ch67\n"
"Ch68\n"
"Ch69\n"
"Ch70\n"
"Ch71\n"
"Ch72\n"
"Ch73\n"
"Ch74\n"
"Ch75\n"
"Ch76\n"
"Ch77\n"
"Ch78\n"
"Ch79\n"
"Ch80\n"
"Ch81\n"
"Ch82\n"
"Ch83\n"
"Ch84\n"
"Ch85\n"
"Ch86\n"
"Ch87\n"
"Ch88\n"
"Ch89\n"
"Ch90\n"
"Ch91\n"
"Ch92\n"
"Ch93\n"
"Ch94\n"
"Ch95\n"
"Ch96\n"
"Ch97\n"
"Ch98\n"
"Ch99\n"
"Ch100\n"
"Ch101\n"
"Ch102\n"
"Ch103\n"
"Ch104\n"
"Ch105\n"
"Ch106\n"
"Ch107\n"
"Ch108\n"
"Ch109\n"
"Ch110\n"
"Ch111\n"
"Ch112\n"
"Ch113\n"
"Ch1"
                        "14\n"
"Ch115\n"
"Ch116\n"
"Ch117\n"
"Ch118\n"
"Ch119\n"
"Ch120\n"
"Ch121\n"
"Ch122\n"
"Ch123\n"
"Ch124\n"
"Ch125\n"
"Ch126\n"
"Ch127\n"
"Ch128\n"
"Ch129\n"
"Ch130\n"
"Ch131\n"
"Ch132\n"
"Ch133\n"
"Ch134\n"
"Ch135\n"
"Ch136\n"
"Ch137\n"
"Ch138\n"
"Ch139\n"
"Ch140\n"
"Ch141\n"
"Ch142\n"
"Ch143\n"
"Ch144\n"
"Ch145\n"
"Ch146\n"
"Ch147\n"
"Ch148\n"
"Ch149\n"
"Ch150\n"
"Ch151\n"
"Ch152\n"
"Ch153\n"
"Ch154\n"
"Ch155\n"
"Ch156\n"
"Ch157\n"
"Ch158\n"
"Ch159\n"
"Ch160\n"
"Ch161\n"
"Ch162\n"
"Ch163\n"
"Ch164\n"
"Ch165\n"
"Ch166\n"
"Ch167\n"
"Ch168\n"
"Ch169\n"
"Ch170\n"
"Ch171\n"
"Ch172\n"
"Ch173\n"
"Ch174\n"
"Ch175\n"
"Ch176\n"
"Ch177\n"
"Ch178\n"
"Ch179\n"
"Ch180\n"
"Ch181\n"
"Ch182\n"
"Ch183\n"
"Ch184\n"
"Ch185\n"
"Ch186\n"
"Ch187\n"
"Ch188\n"
"Ch189\n"
"Ch190\n"
"Ch191\n"
"Ch192\n"
"Ch193\n"
"Ch194\n"
"Ch195\n"
"Ch196\n"
"Ch197\n"
"Ch198\n"
"Ch199\n"
"Ch200\n"
"Ch201\n"
"Ch202\n"
"Ch203\n"
"Ch204\n"
"Ch205\n"
"Ch206\n"
"Ch207\n"
"Ch208\n"
"Ch209\n"
"Ch210\n"
"Ch211\n"
"Ch212\n"
"Ch213\n"
"Ch214\n"
"Ch215\n"
"Ch216\n"
""
                        "Ch217\n"
"Ch218\n"
"Ch219\n"
"Ch220\n"
"Ch221\n"
"Ch222\n"
"Ch223\n"
"Ch224\n"
"Ch225\n"
"Ch226\n"
"Ch227\n"
"Ch228\n"
"Ch229\n"
"Ch230\n"
"Ch231\n"
"Ch232\n"
"Ch233\n"
"Ch234\n"
"Ch235\n"
"Ch236\n"
"Ch237\n"
"Ch238\n"
"Ch239\n"
"Ch240\n"
"Ch241\n"
"Ch242\n"
"Ch243\n"
"Ch244\n"
"Ch245\n"
"Ch246\n"
"Ch247\n"
"Ch248\n"
"Ch249\n"
"Ch250\n"
"Ch251\n"
"Ch252\n"
"Ch253\n"
"Ch254\n"
"Ch255\n"
"Ch256", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Device Settings", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Device Port or Serial", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        deviceNumber->setToolTip(QApplication::translate("MainWindow", "This is either the USB port number of the device, or the serial number of the device (a string of the form UX-XXXX.XX.XX). If left as 0, all USB ports will be searched.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        deviceNumber->setText(QApplication::translate("MainWindow", "0", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Number of Channels", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        channelCount->setToolTip(QApplication::translate("MainWindow", "This must match the number of entries in the channel list", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("MainWindow", "Chunk Size", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        chunkSize->setToolTip(QApplication::translate("MainWindow", "The number of samples per chunk emitted by the driver -- a small value will lead to lower overall latency but causes more CPU load. The official recommendation is to use a power of two that is closest to sampling rate / 32.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("MainWindow", "Sampling Rate", 0, QApplication::UnicodeUTF8));
        samplingRate->clear();
        samplingRate->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "256", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "512", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "600", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1200", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "2400", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "4800", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "9600", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "19200", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "38400", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        samplingRate->setToolTip(QApplication::translate("MainWindow", "Sampling rate to use.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        isSlave->setToolTip(QApplication::translate("MainWindow", "Note: this is highly experimental -- can be used to sync multiple amps with a trigger cable. This needs to be checked for all slave devices, and unchecked for the 'master' device. The master must be linked/unlinked last (i.e., after all the slaves).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        isSlave->setText(QApplication::translate("MainWindow", "(check)", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "Act as Slave", 0, QApplication::UnicodeUTF8));
        linkButton->setText(QApplication::translate("MainWindow", "Link", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
