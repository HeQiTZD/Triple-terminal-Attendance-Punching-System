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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
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
    QLabel *deviceStatusLabel;
    QPushButton *settingButton;
    QHBoxLayout *horizontalLayout_center;
    QWidget *leftWidget;
    QVBoxLayout *verticalLayout_left;
    QGroupBox *infoGroupBox;
    QVBoxLayout *verticalLayout_info;
    QHBoxLayout *horizontalLayout_employeeId;
    QLabel *employeeIdLabel;
    QLineEdit *employeeIdEdit;
    QHBoxLayout *horizontalLayout_name;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QHBoxLayout *horizontalLayout_status;
    QLabel *statusLabel;
    QLineEdit *statusEdit;
    QHBoxLayout *horizontalLayout_time;
    QLabel *checkTimeLabel;
    QLineEdit *checkTimeEdit;
    QGroupBox *faceImageGroupBox;
    QVBoxLayout *verticalLayout_face;
    QLabel *faceImageLabel;
    QWidget *rightWidget;
    QVBoxLayout *verticalLayout_right;
    QLabel *cameraLabel;
    QLabel *cameraDisplay;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1299, 892);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_main = new QVBoxLayout(centralwidget);
        verticalLayout_main->setObjectName("verticalLayout_main");
        topWidget = new QWidget(centralwidget);
        topWidget->setObjectName("topWidget");
        horizontalLayout_top = new QHBoxLayout(topWidget);
        horizontalLayout_top->setObjectName("horizontalLayout_top");
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
        networkStatusLabel->setStyleSheet(QString::fromUtf8("color: green;"));

        horizontalLayout_top->addWidget(networkStatusLabel);

        deviceStatusLabel = new QLabel(topWidget);
        deviceStatusLabel->setObjectName("deviceStatusLabel");
        deviceStatusLabel->setStyleSheet(QString::fromUtf8("color: green;"));

        horizontalLayout_top->addWidget(deviceStatusLabel);

        settingButton = new QPushButton(topWidget);
        settingButton->setObjectName("settingButton");
        QIcon icon(QIcon::fromTheme(QString::fromUtf8("configure")));
        settingButton->setIcon(icon);

        horizontalLayout_top->addWidget(settingButton);


        verticalLayout_main->addWidget(topWidget);

        horizontalLayout_center = new QHBoxLayout();
        horizontalLayout_center->setObjectName("horizontalLayout_center");
        leftWidget = new QWidget(centralwidget);
        leftWidget->setObjectName("leftWidget");
        verticalLayout_left = new QVBoxLayout(leftWidget);
        verticalLayout_left->setObjectName("verticalLayout_left");
        infoGroupBox = new QGroupBox(leftWidget);
        infoGroupBox->setObjectName("infoGroupBox");
        verticalLayout_info = new QVBoxLayout(infoGroupBox);
        verticalLayout_info->setObjectName("verticalLayout_info");
        horizontalLayout_employeeId = new QHBoxLayout();
        horizontalLayout_employeeId->setObjectName("horizontalLayout_employeeId");
        employeeIdLabel = new QLabel(infoGroupBox);
        employeeIdLabel->setObjectName("employeeIdLabel");

        horizontalLayout_employeeId->addWidget(employeeIdLabel);

        employeeIdEdit = new QLineEdit(infoGroupBox);
        employeeIdEdit->setObjectName("employeeIdEdit");
        employeeIdEdit->setReadOnly(true);

        horizontalLayout_employeeId->addWidget(employeeIdEdit);


        verticalLayout_info->addLayout(horizontalLayout_employeeId);

        horizontalLayout_name = new QHBoxLayout();
        horizontalLayout_name->setObjectName("horizontalLayout_name");
        nameLabel = new QLabel(infoGroupBox);
        nameLabel->setObjectName("nameLabel");

        horizontalLayout_name->addWidget(nameLabel);

        nameEdit = new QLineEdit(infoGroupBox);
        nameEdit->setObjectName("nameEdit");
        nameEdit->setReadOnly(true);

        horizontalLayout_name->addWidget(nameEdit);


        verticalLayout_info->addLayout(horizontalLayout_name);

        horizontalLayout_status = new QHBoxLayout();
        horizontalLayout_status->setObjectName("horizontalLayout_status");
        statusLabel = new QLabel(infoGroupBox);
        statusLabel->setObjectName("statusLabel");

        horizontalLayout_status->addWidget(statusLabel);

        statusEdit = new QLineEdit(infoGroupBox);
        statusEdit->setObjectName("statusEdit");
        statusEdit->setReadOnly(true);

        horizontalLayout_status->addWidget(statusEdit);


        verticalLayout_info->addLayout(horizontalLayout_status);

        horizontalLayout_time = new QHBoxLayout();
        horizontalLayout_time->setObjectName("horizontalLayout_time");
        checkTimeLabel = new QLabel(infoGroupBox);
        checkTimeLabel->setObjectName("checkTimeLabel");

        horizontalLayout_time->addWidget(checkTimeLabel);

        checkTimeEdit = new QLineEdit(infoGroupBox);
        checkTimeEdit->setObjectName("checkTimeEdit");
        checkTimeEdit->setReadOnly(true);

        horizontalLayout_time->addWidget(checkTimeEdit);


        verticalLayout_info->addLayout(horizontalLayout_time);


        verticalLayout_left->addWidget(infoGroupBox);

        faceImageGroupBox = new QGroupBox(leftWidget);
        faceImageGroupBox->setObjectName("faceImageGroupBox");
        verticalLayout_face = new QVBoxLayout(faceImageGroupBox);
        verticalLayout_face->setObjectName("verticalLayout_face");
        faceImageLabel = new QLabel(faceImageGroupBox);
        faceImageLabel->setObjectName("faceImageLabel");
        faceImageLabel->setMinimumSize(QSize(200, 200));
        faceImageLabel->setStyleSheet(QString::fromUtf8("background-color: lightgray; border: 1px solid gray;"));
        faceImageLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_face->addWidget(faceImageLabel);


        verticalLayout_left->addWidget(faceImageGroupBox);


        horizontalLayout_center->addWidget(leftWidget);

        rightWidget = new QWidget(centralwidget);
        rightWidget->setObjectName("rightWidget");
        verticalLayout_right = new QVBoxLayout(rightWidget);
        verticalLayout_right->setObjectName("verticalLayout_right");
        cameraLabel = new QLabel(rightWidget);
        cameraLabel->setObjectName("cameraLabel");
        cameraLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_right->addWidget(cameraLabel);

        cameraDisplay = new QLabel(rightWidget);
        cameraDisplay->setObjectName("cameraDisplay");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cameraDisplay->sizePolicy().hasHeightForWidth());
        cameraDisplay->setSizePolicy(sizePolicy);
        cameraDisplay->setMinimumSize(QSize(640, 480));
        cameraDisplay->setStyleSheet(QString::fromUtf8("background-color: black; color: white; border: 2px solid gray;"));
        cameraDisplay->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_right->addWidget(cameraDisplay);


        horizontalLayout_center->addWidget(rightWidget);

        horizontalLayout_center->setStretch(0, 1);
        horizontalLayout_center->setStretch(1, 2);

        verticalLayout_main->addLayout(horizontalLayout_center);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        statusbar->setStyleSheet(QString::fromUtf8("QStatusBar { background-color: #f0f0f0; }"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\344\272\272\350\204\270\350\257\206\345\210\253\350\200\203\345\213\244\347\263\273\347\273\237", nullptr));
        timeLabel->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\346\227\266\351\227\264\357\274\2322024-01-01 12:00:00", nullptr));
        networkStatusLabel->setText(QCoreApplication::translate("MainWindow", "\347\275\221\347\273\234\347\212\266\346\200\201\357\274\232\342\227\217", nullptr));
        deviceStatusLabel->setText(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207\347\212\266\346\200\201\357\274\232\342\227\217", nullptr));
        settingButton->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256", nullptr));
        infoGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\345\221\230\345\267\245\344\277\241\346\201\257", nullptr));
        employeeIdLabel->setText(QCoreApplication::translate("MainWindow", "\345\221\230\345\267\245\345\217\267\357\274\232", nullptr));
        nameLabel->setText(QCoreApplication::translate("MainWindow", "\345\247\223\345\220\215\357\274\232", nullptr));
        statusLabel->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\215\241\347\212\266\346\200\201\357\274\232", nullptr));
        checkTimeLabel->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\215\241\346\227\266\351\227\264\357\274\232", nullptr));
        faceImageGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\350\257\206\345\210\253\345\210\260\347\232\204\344\272\272\350\204\270", nullptr));
        faceImageLabel->setText(QCoreApplication::translate("MainWindow", "\346\232\202\346\227\240\344\272\272\350\204\270\350\257\206\345\210\253", nullptr));
        cameraLabel->setText(QCoreApplication::translate("MainWindow", "\346\221\204\345\203\217\345\244\264\345\256\236\346\227\266\347\224\273\351\235\242", nullptr));
        cameraDisplay->setText(QCoreApplication::translate("MainWindow", "\346\221\204\345\203\217\345\244\264\347\224\273\351\235\242\346\230\276\347\244\272\345\214\272\345\237\237", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
