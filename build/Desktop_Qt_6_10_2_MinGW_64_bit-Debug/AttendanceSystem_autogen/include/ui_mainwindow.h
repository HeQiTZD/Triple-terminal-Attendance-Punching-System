/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_main;
    QWidget *topWidget;
    QHBoxLayout *horizontalLayout_top;
    QLabel *timeLabel;
    QSpacerItem *horizontalSpacer_top;
    QLabel *networkStatusLabel;
    QPushButton *settingButton;
    QPushButton *minimizeButton;
    QPushButton *maximizeButton;
    QPushButton *closeButton;
    QVBoxLayout *verticalLayout_content;
    QWidget *infoWidget;
    QHBoxLayout *horizontalLayout_info;
    QLabel *employeeIdLabel;
    QLabel *employeeIdEdit;
    QSpacerItem *horizontalSpacer_1;
    QLabel *nameLabel;
    QLabel *nameEdit;
    QSpacerItem *horizontalSpacer_2;
    QLabel *statusLabel;
    QLabel *statusEdit;
    QSpacerItem *horizontalSpacer_3;
    QLabel *checkTimeLabel;
    QLabel *checkTimeEdit;
    QWidget *cameraWidget;
    QVBoxLayout *verticalLayout_camera;
    QLabel *cameraLabel;
    QLabel *cameraDisplay;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1299, 892);
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow { background-color: #0d1117; }"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setStyleSheet(QString::fromUtf8("QWidget#centralwidget { background-color: #0d1117; }"));
        verticalLayout_main = new QVBoxLayout(centralwidget);
        verticalLayout_main->setSpacing(0);
        verticalLayout_main->setObjectName("verticalLayout_main");
        verticalLayout_main->setContentsMargins(0, 0, 0, 0);
        topWidget = new QWidget(centralwidget);
        topWidget->setObjectName("topWidget");
        topWidget->setMinimumSize(QSize(0, 52));
        horizontalLayout_top = new QHBoxLayout(topWidget);
        horizontalLayout_top->setObjectName("horizontalLayout_top");
        horizontalLayout_top->setContentsMargins(15, 8, 8, 8);
        timeLabel = new QLabel(topWidget);
        timeLabel->setObjectName("timeLabel");
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        timeLabel->setFont(font);

        horizontalLayout_top->addWidget(timeLabel);

        horizontalSpacer_top = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_top->addItem(horizontalSpacer_top);

        networkStatusLabel = new QLabel(topWidget);
        networkStatusLabel->setObjectName("networkStatusLabel");

        horizontalLayout_top->addWidget(networkStatusLabel);

        settingButton = new QPushButton(topWidget);
        settingButton->setObjectName("settingButton");

        horizontalLayout_top->addWidget(settingButton);

        minimizeButton = new QPushButton(topWidget);
        minimizeButton->setObjectName("minimizeButton");

        horizontalLayout_top->addWidget(minimizeButton);

        maximizeButton = new QPushButton(topWidget);
        maximizeButton->setObjectName("maximizeButton");

        horizontalLayout_top->addWidget(maximizeButton);

        closeButton = new QPushButton(topWidget);
        closeButton->setObjectName("closeButton");

        horizontalLayout_top->addWidget(closeButton);


        verticalLayout_main->addWidget(topWidget);

        verticalLayout_content = new QVBoxLayout();
        verticalLayout_content->setSpacing(8);
        verticalLayout_content->setObjectName("verticalLayout_content");
        verticalLayout_content->setContentsMargins(8, 8, 8, 8);
        infoWidget = new QWidget(centralwidget);
        infoWidget->setObjectName("infoWidget");
        infoWidget->setMaximumSize(QSize(16777215, 90));
        horizontalLayout_info = new QHBoxLayout(infoWidget);
        horizontalLayout_info->setSpacing(24);
        horizontalLayout_info->setObjectName("horizontalLayout_info");
        horizontalLayout_info->setContentsMargins(16, 8, 16, 8);
        employeeIdLabel = new QLabel(infoWidget);
        employeeIdLabel->setObjectName("employeeIdLabel");

        horizontalLayout_info->addWidget(employeeIdLabel);

        employeeIdEdit = new QLabel(infoWidget);
        employeeIdEdit->setObjectName("employeeIdEdit");

        horizontalLayout_info->addWidget(employeeIdEdit);

        horizontalSpacer_1 = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_info->addItem(horizontalSpacer_1);

        nameLabel = new QLabel(infoWidget);
        nameLabel->setObjectName("nameLabel");

        horizontalLayout_info->addWidget(nameLabel);

        nameEdit = new QLabel(infoWidget);
        nameEdit->setObjectName("nameEdit");

        horizontalLayout_info->addWidget(nameEdit);

        horizontalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_info->addItem(horizontalSpacer_2);

        statusLabel = new QLabel(infoWidget);
        statusLabel->setObjectName("statusLabel");

        horizontalLayout_info->addWidget(statusLabel);

        statusEdit = new QLabel(infoWidget);
        statusEdit->setObjectName("statusEdit");

        horizontalLayout_info->addWidget(statusEdit);

        horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_info->addItem(horizontalSpacer_3);

        checkTimeLabel = new QLabel(infoWidget);
        checkTimeLabel->setObjectName("checkTimeLabel");

        horizontalLayout_info->addWidget(checkTimeLabel);

        checkTimeEdit = new QLabel(infoWidget);
        checkTimeEdit->setObjectName("checkTimeEdit");

        horizontalLayout_info->addWidget(checkTimeEdit);


        verticalLayout_content->addWidget(infoWidget);

        cameraWidget = new QWidget(centralwidget);
        cameraWidget->setObjectName("cameraWidget");
        verticalLayout_camera = new QVBoxLayout(cameraWidget);
        verticalLayout_camera->setSpacing(4);
        verticalLayout_camera->setObjectName("verticalLayout_camera");
        verticalLayout_camera->setContentsMargins(0, 0, 0, 0);
        cameraLabel = new QLabel(cameraWidget);
        cameraLabel->setObjectName("cameraLabel");
        cameraLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout_camera->addWidget(cameraLabel);

        cameraDisplay = new QLabel(cameraWidget);
        cameraDisplay->setObjectName("cameraDisplay");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cameraDisplay->sizePolicy().hasHeightForWidth());
        cameraDisplay->setSizePolicy(sizePolicy);
        cameraDisplay->setMinimumSize(QSize(640, 480));
        cameraDisplay->setStyleSheet(QString::fromUtf8("background-color: #0a0e14; color: #30363d; border: 2px solid #00d4ff;"));
        cameraDisplay->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_camera->addWidget(cameraDisplay);


        verticalLayout_content->addWidget(cameraWidget);


        verticalLayout_main->addLayout(verticalLayout_content);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\344\272\272\350\204\270\350\257\206\345\210\253\350\200\203\345\213\244\347\263\273\347\273\237", nullptr));
        timeLabel->setText(QCoreApplication::translate("MainWindow", "2024-01-01  12:00:00", nullptr));
        networkStatusLabel->setText(QCoreApplication::translate("MainWindow", "\360\237\224\264 \347\246\273\347\272\277", nullptr));
        settingButton->setText(QCoreApplication::translate("MainWindow", "\342\232\231  \350\256\276\347\275\256", nullptr));
        minimizeButton->setText(QCoreApplication::translate("MainWindow", "\342\210\222", nullptr));
        maximizeButton->setText(QCoreApplication::translate("MainWindow", "\342\254\234", nullptr));
        closeButton->setText(QCoreApplication::translate("MainWindow", "\342\234\225", nullptr));
        employeeIdLabel->setText(QCoreApplication::translate("MainWindow", "\345\221\230\345\267\245\345\217\267\357\274\232", nullptr));
        employeeIdEdit->setText(QString());
        nameLabel->setText(QCoreApplication::translate("MainWindow", "\345\247\223\345\220\215\357\274\232", nullptr));
        nameEdit->setText(QString());
        statusLabel->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\215\241\347\212\266\346\200\201\357\274\232", nullptr));
        statusEdit->setText(QString());
        checkTimeLabel->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\215\241\346\227\266\351\227\264\357\274\232", nullptr));
        checkTimeEdit->setText(QString());
        cameraLabel->setText(QCoreApplication::translate("MainWindow", "\342\226\266  \345\256\236\346\227\266\347\224\273\351\235\242", nullptr));
        cameraDisplay->setText(QCoreApplication::translate("MainWindow", "\346\221\204\345\203\217\345\244\264\347\224\273\351\235\242\346\230\276\347\244\272\345\214\272\345\237\237", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
