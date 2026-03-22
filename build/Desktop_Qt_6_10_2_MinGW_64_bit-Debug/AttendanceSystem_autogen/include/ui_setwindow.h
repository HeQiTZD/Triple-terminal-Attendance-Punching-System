/********************************************************************************
** Form generated from reading UI file 'setwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETWINDOW_H
#define UI_SETWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SetWindow
{
public:
    QHBoxLayout *mainLayout;
    QWidget *navWidget;
    QVBoxLayout *navLayout;
    QPushButton *btnNetwork;
    QPushButton *btnFace;
    QPushButton *btnAttendance;
    QPushButton *btnStorage;
    QSpacerItem *navSpacer;
    QWidget *contentWidget;
    QVBoxLayout *contentLayout;
    QScrollArea *scrollArea;
    QWidget *scrollContent;
    QVBoxLayout *scrollLayout;
    QGroupBox *networkGroup;
    QVBoxLayout *networkLayout;
    QHBoxLayout *ipLayout;
    QLabel *ipLabel;
    QLineEdit *lineEditIP;
    QLabel *portLabel;
    QSpinBox *spinBoxPort;
    QHBoxLayout *timeoutLayout;
    QLabel *timeoutLabel;
    QSpinBox *spinBoxTimeout;
    QSpacerItem *timeoutSpacer;
    QHBoxLayout *testLayout;
    QPushButton *btnTestConnection;
    QLabel *labelConnectionStatus;
    QSpacerItem *testSpacer;
    QGroupBox *faceGroup;
    QVBoxLayout *faceLayout;
    QHBoxLayout *thresholdLayout;
    QLabel *thresholdLabel;
    QSlider *sliderThreshold;
    QLabel *labelThresholdValue;
    QHBoxLayout *maxFaceLayout;
    QLabel *maxFaceLabel;
    QSpinBox *spinBoxMaxFace;
    QSpacerItem *maxFaceSpacer;
    QHBoxLayout *recognizeTimeoutLayout;
    QLabel *recognizeTimeoutLabel;
    QSpinBox *spinBoxRecognizeTimeout;
    QSpacerItem *recognizeTimeoutSpacer;
    QGroupBox *attendanceGroup;
    QVBoxLayout *attendanceLayout;
    QHBoxLayout *workTimeLayout;
    QLabel *workStartLabel;
    QTimeEdit *timeEditWorkStart;
    QLabel *workEndLabel;
    QTimeEdit *timeEditWorkEnd;
    QSpacerItem *workTimeSpacer;
    QHBoxLayout *lateLayout;
    QLabel *lateLabel;
    QSpinBox *spinBoxLate;
    QSpacerItem *lateSpacer;
    QHBoxLayout *earlyLayout;
    QLabel *earlyLabel;
    QSpinBox *spinBoxEarly;
    QSpacerItem *earlySpacer;
    QGroupBox *storageGroup;
    QVBoxLayout *storageLayout;
    QHBoxLayout *dbPathLayout;
    QLabel *dbPathLabel;
    QLineEdit *lineEditDbPath;
    QPushButton *btnBrowseDb;
    QHBoxLayout *logPathLayout;
    QLabel *logPathLabel;
    QLineEdit *lineEditLogPath;
    QPushButton *btnBrowseLog;
    QHBoxLayout *cachePathLayout;
    QLabel *cachePathLabel;
    QLineEdit *lineEditCachePath;
    QPushButton *btnBrowseCache;
    QSpacerItem *bottomSpacer;
    QHBoxLayout *buttonLayout;
    QPushButton *btnRestore;
    QSpacerItem *buttonSpacer;
    QPushButton *btnCancel;
    QPushButton *btnSave;

    void setupUi(QWidget *SetWindow)
    {
        if (SetWindow->objectName().isEmpty())
            SetWindow->setObjectName("SetWindow");
        SetWindow->resize(700, 600);
        SetWindow->setMinimumSize(QSize(700, 600));
        mainLayout = new QHBoxLayout(SetWindow);
        mainLayout->setSpacing(0);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(0, 0, 0, 0);
        navWidget = new QWidget(SetWindow);
        navWidget->setObjectName("navWidget");
        navWidget->setMinimumSize(QSize(150, 0));
        navWidget->setMaximumSize(QSize(150, 16777215));
        navLayout = new QVBoxLayout(navWidget);
        navLayout->setSpacing(5);
        navLayout->setObjectName("navLayout");
        navLayout->setContentsMargins(10, 20, 10, 20);
        btnNetwork = new QPushButton(navWidget);
        btnNetwork->setObjectName("btnNetwork");
        btnNetwork->setMinimumSize(QSize(0, 45));
        btnNetwork->setCheckable(true);
        btnNetwork->setChecked(true);
        btnNetwork->setAutoExclusive(true);

        navLayout->addWidget(btnNetwork);

        btnFace = new QPushButton(navWidget);
        btnFace->setObjectName("btnFace");
        btnFace->setMinimumSize(QSize(0, 45));
        btnFace->setCheckable(true);
        btnFace->setAutoExclusive(true);

        navLayout->addWidget(btnFace);

        btnAttendance = new QPushButton(navWidget);
        btnAttendance->setObjectName("btnAttendance");
        btnAttendance->setMinimumSize(QSize(0, 45));
        btnAttendance->setCheckable(true);
        btnAttendance->setAutoExclusive(true);

        navLayout->addWidget(btnAttendance);

        btnStorage = new QPushButton(navWidget);
        btnStorage->setObjectName("btnStorage");
        btnStorage->setMinimumSize(QSize(0, 45));
        btnStorage->setCheckable(true);
        btnStorage->setAutoExclusive(true);

        navLayout->addWidget(btnStorage);

        navSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        navLayout->addItem(navSpacer);


        mainLayout->addWidget(navWidget);

        contentWidget = new QWidget(SetWindow);
        contentWidget->setObjectName("contentWidget");
        contentLayout = new QVBoxLayout(contentWidget);
        contentLayout->setSpacing(15);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(30, 20, 30, 20);
        scrollArea = new QScrollArea(contentWidget);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        scrollArea->setWidgetResizable(true);
        scrollContent = new QWidget();
        scrollContent->setObjectName("scrollContent");
        scrollContent->setGeometry(QRect(0, 0, 488, 900));
        scrollLayout = new QVBoxLayout(scrollContent);
        scrollLayout->setSpacing(20);
        scrollLayout->setObjectName("scrollLayout");
        scrollLayout->setContentsMargins(0, 0, 20, 0);
        networkGroup = new QGroupBox(scrollContent);
        networkGroup->setObjectName("networkGroup");
        networkLayout = new QVBoxLayout(networkGroup);
        networkLayout->setSpacing(15);
        networkLayout->setObjectName("networkLayout");
        ipLayout = new QHBoxLayout();
        ipLayout->setObjectName("ipLayout");
        ipLabel = new QLabel(networkGroup);
        ipLabel->setObjectName("ipLabel");
        ipLabel->setMinimumSize(QSize(100, 0));

        ipLayout->addWidget(ipLabel);

        lineEditIP = new QLineEdit(networkGroup);
        lineEditIP->setObjectName("lineEditIP");

        ipLayout->addWidget(lineEditIP);

        portLabel = new QLabel(networkGroup);
        portLabel->setObjectName("portLabel");

        ipLayout->addWidget(portLabel);

        spinBoxPort = new QSpinBox(networkGroup);
        spinBoxPort->setObjectName("spinBoxPort");
        spinBoxPort->setMinimumSize(QSize(80, 0));
        spinBoxPort->setMinimum(1);
        spinBoxPort->setMaximum(65535);
        spinBoxPort->setValue(8080);

        ipLayout->addWidget(spinBoxPort);


        networkLayout->addLayout(ipLayout);

        timeoutLayout = new QHBoxLayout();
        timeoutLayout->setObjectName("timeoutLayout");
        timeoutLabel = new QLabel(networkGroup);
        timeoutLabel->setObjectName("timeoutLabel");
        timeoutLabel->setMinimumSize(QSize(100, 0));

        timeoutLayout->addWidget(timeoutLabel);

        spinBoxTimeout = new QSpinBox(networkGroup);
        spinBoxTimeout->setObjectName("spinBoxTimeout");
        spinBoxTimeout->setMinimum(5);
        spinBoxTimeout->setMaximum(60);
        spinBoxTimeout->setValue(30);

        timeoutLayout->addWidget(spinBoxTimeout);

        timeoutSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        timeoutLayout->addItem(timeoutSpacer);


        networkLayout->addLayout(timeoutLayout);

        testLayout = new QHBoxLayout();
        testLayout->setObjectName("testLayout");
        btnTestConnection = new QPushButton(networkGroup);
        btnTestConnection->setObjectName("btnTestConnection");

        testLayout->addWidget(btnTestConnection);

        labelConnectionStatus = new QLabel(networkGroup);
        labelConnectionStatus->setObjectName("labelConnectionStatus");

        testLayout->addWidget(labelConnectionStatus);

        testSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        testLayout->addItem(testSpacer);


        networkLayout->addLayout(testLayout);


        scrollLayout->addWidget(networkGroup);

        faceGroup = new QGroupBox(scrollContent);
        faceGroup->setObjectName("faceGroup");
        faceLayout = new QVBoxLayout(faceGroup);
        faceLayout->setSpacing(15);
        faceLayout->setObjectName("faceLayout");
        thresholdLayout = new QHBoxLayout();
        thresholdLayout->setObjectName("thresholdLayout");
        thresholdLabel = new QLabel(faceGroup);
        thresholdLabel->setObjectName("thresholdLabel");
        thresholdLabel->setMinimumSize(QSize(120, 0));

        thresholdLayout->addWidget(thresholdLabel);

        sliderThreshold = new QSlider(faceGroup);
        sliderThreshold->setObjectName("sliderThreshold");
        sliderThreshold->setMinimumSize(QSize(200, 0));
        sliderThreshold->setMinimum(50);
        sliderThreshold->setMaximum(99);
        sliderThreshold->setValue(80);
        sliderThreshold->setOrientation(Qt::Orientation::Horizontal);

        thresholdLayout->addWidget(sliderThreshold);

        labelThresholdValue = new QLabel(faceGroup);
        labelThresholdValue->setObjectName("labelThresholdValue");
        labelThresholdValue->setMinimumSize(QSize(50, 0));
        labelThresholdValue->setAlignment(Qt::AlignmentFlag::AlignCenter);

        thresholdLayout->addWidget(labelThresholdValue);


        faceLayout->addLayout(thresholdLayout);

        maxFaceLayout = new QHBoxLayout();
        maxFaceLayout->setObjectName("maxFaceLayout");
        maxFaceLabel = new QLabel(faceGroup);
        maxFaceLabel->setObjectName("maxFaceLabel");
        maxFaceLabel->setMinimumSize(QSize(120, 0));

        maxFaceLayout->addWidget(maxFaceLabel);

        spinBoxMaxFace = new QSpinBox(faceGroup);
        spinBoxMaxFace->setObjectName("spinBoxMaxFace");
        spinBoxMaxFace->setMinimum(1);
        spinBoxMaxFace->setMaximum(10);
        spinBoxMaxFace->setValue(5);

        maxFaceLayout->addWidget(spinBoxMaxFace);

        maxFaceSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        maxFaceLayout->addItem(maxFaceSpacer);


        faceLayout->addLayout(maxFaceLayout);

        recognizeTimeoutLayout = new QHBoxLayout();
        recognizeTimeoutLayout->setObjectName("recognizeTimeoutLayout");
        recognizeTimeoutLabel = new QLabel(faceGroup);
        recognizeTimeoutLabel->setObjectName("recognizeTimeoutLabel");
        recognizeTimeoutLabel->setMinimumSize(QSize(120, 0));

        recognizeTimeoutLayout->addWidget(recognizeTimeoutLabel);

        spinBoxRecognizeTimeout = new QSpinBox(faceGroup);
        spinBoxRecognizeTimeout->setObjectName("spinBoxRecognizeTimeout");
        spinBoxRecognizeTimeout->setMinimum(3);
        spinBoxRecognizeTimeout->setMaximum(30);
        spinBoxRecognizeTimeout->setValue(10);

        recognizeTimeoutLayout->addWidget(spinBoxRecognizeTimeout);

        recognizeTimeoutSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        recognizeTimeoutLayout->addItem(recognizeTimeoutSpacer);


        faceLayout->addLayout(recognizeTimeoutLayout);


        scrollLayout->addWidget(faceGroup);

        attendanceGroup = new QGroupBox(scrollContent);
        attendanceGroup->setObjectName("attendanceGroup");
        attendanceLayout = new QVBoxLayout(attendanceGroup);
        attendanceLayout->setSpacing(15);
        attendanceLayout->setObjectName("attendanceLayout");
        workTimeLayout = new QHBoxLayout();
        workTimeLayout->setObjectName("workTimeLayout");
        workStartLabel = new QLabel(attendanceGroup);
        workStartLabel->setObjectName("workStartLabel");
        workStartLabel->setMinimumSize(QSize(100, 0));

        workTimeLayout->addWidget(workStartLabel);

        timeEditWorkStart = new QTimeEdit(attendanceGroup);
        timeEditWorkStart->setObjectName("timeEditWorkStart");
        timeEditWorkStart->setTime(QTime(9, 0, 0));

        workTimeLayout->addWidget(timeEditWorkStart);

        workEndLabel = new QLabel(attendanceGroup);
        workEndLabel->setObjectName("workEndLabel");

        workTimeLayout->addWidget(workEndLabel);

        timeEditWorkEnd = new QTimeEdit(attendanceGroup);
        timeEditWorkEnd->setObjectName("timeEditWorkEnd");
        timeEditWorkEnd->setTime(QTime(18, 0, 0));

        workTimeLayout->addWidget(timeEditWorkEnd);

        workTimeSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        workTimeLayout->addItem(workTimeSpacer);


        attendanceLayout->addLayout(workTimeLayout);

        lateLayout = new QHBoxLayout();
        lateLayout->setObjectName("lateLayout");
        lateLabel = new QLabel(attendanceGroup);
        lateLabel->setObjectName("lateLabel");
        lateLabel->setMinimumSize(QSize(100, 0));

        lateLayout->addWidget(lateLabel);

        spinBoxLate = new QSpinBox(attendanceGroup);
        spinBoxLate->setObjectName("spinBoxLate");
        spinBoxLate->setMinimum(0);
        spinBoxLate->setMaximum(60);
        spinBoxLate->setValue(15);

        lateLayout->addWidget(spinBoxLate);

        lateSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        lateLayout->addItem(lateSpacer);


        attendanceLayout->addLayout(lateLayout);

        earlyLayout = new QHBoxLayout();
        earlyLayout->setObjectName("earlyLayout");
        earlyLabel = new QLabel(attendanceGroup);
        earlyLabel->setObjectName("earlyLabel");
        earlyLabel->setMinimumSize(QSize(100, 0));

        earlyLayout->addWidget(earlyLabel);

        spinBoxEarly = new QSpinBox(attendanceGroup);
        spinBoxEarly->setObjectName("spinBoxEarly");
        spinBoxEarly->setMinimum(0);
        spinBoxEarly->setMaximum(60);
        spinBoxEarly->setValue(15);

        earlyLayout->addWidget(spinBoxEarly);

        earlySpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        earlyLayout->addItem(earlySpacer);


        attendanceLayout->addLayout(earlyLayout);


        scrollLayout->addWidget(attendanceGroup);

        storageGroup = new QGroupBox(scrollContent);
        storageGroup->setObjectName("storageGroup");
        storageLayout = new QVBoxLayout(storageGroup);
        storageLayout->setSpacing(15);
        storageLayout->setObjectName("storageLayout");
        dbPathLayout = new QHBoxLayout();
        dbPathLayout->setObjectName("dbPathLayout");
        dbPathLabel = new QLabel(storageGroup);
        dbPathLabel->setObjectName("dbPathLabel");
        dbPathLabel->setMinimumSize(QSize(100, 0));

        dbPathLayout->addWidget(dbPathLabel);

        lineEditDbPath = new QLineEdit(storageGroup);
        lineEditDbPath->setObjectName("lineEditDbPath");
        lineEditDbPath->setReadOnly(true);

        dbPathLayout->addWidget(lineEditDbPath);

        btnBrowseDb = new QPushButton(storageGroup);
        btnBrowseDb->setObjectName("btnBrowseDb");

        dbPathLayout->addWidget(btnBrowseDb);


        storageLayout->addLayout(dbPathLayout);

        logPathLayout = new QHBoxLayout();
        logPathLayout->setObjectName("logPathLayout");
        logPathLabel = new QLabel(storageGroup);
        logPathLabel->setObjectName("logPathLabel");
        logPathLabel->setMinimumSize(QSize(100, 0));

        logPathLayout->addWidget(logPathLabel);

        lineEditLogPath = new QLineEdit(storageGroup);
        lineEditLogPath->setObjectName("lineEditLogPath");
        lineEditLogPath->setReadOnly(true);

        logPathLayout->addWidget(lineEditLogPath);

        btnBrowseLog = new QPushButton(storageGroup);
        btnBrowseLog->setObjectName("btnBrowseLog");

        logPathLayout->addWidget(btnBrowseLog);


        storageLayout->addLayout(logPathLayout);

        cachePathLayout = new QHBoxLayout();
        cachePathLayout->setObjectName("cachePathLayout");
        cachePathLabel = new QLabel(storageGroup);
        cachePathLabel->setObjectName("cachePathLabel");
        cachePathLabel->setMinimumSize(QSize(100, 0));

        cachePathLayout->addWidget(cachePathLabel);

        lineEditCachePath = new QLineEdit(storageGroup);
        lineEditCachePath->setObjectName("lineEditCachePath");
        lineEditCachePath->setReadOnly(true);

        cachePathLayout->addWidget(lineEditCachePath);

        btnBrowseCache = new QPushButton(storageGroup);
        btnBrowseCache->setObjectName("btnBrowseCache");

        cachePathLayout->addWidget(btnBrowseCache);


        storageLayout->addLayout(cachePathLayout);


        scrollLayout->addWidget(storageGroup);

        bottomSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        scrollLayout->addItem(bottomSpacer);

        scrollArea->setWidget(scrollContent);

        contentLayout->addWidget(scrollArea);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName("buttonLayout");
        btnRestore = new QPushButton(contentWidget);
        btnRestore->setObjectName("btnRestore");

        buttonLayout->addWidget(btnRestore);

        buttonSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        buttonLayout->addItem(buttonSpacer);

        btnCancel = new QPushButton(contentWidget);
        btnCancel->setObjectName("btnCancel");

        buttonLayout->addWidget(btnCancel);

        btnSave = new QPushButton(contentWidget);
        btnSave->setObjectName("btnSave");

        buttonLayout->addWidget(btnSave);


        contentLayout->addLayout(buttonLayout);


        mainLayout->addWidget(contentWidget);

        mainLayout->setStretch(1, 1);

        retranslateUi(SetWindow);

        QMetaObject::connectSlotsByName(SetWindow);
    } // setupUi

    void retranslateUi(QWidget *SetWindow)
    {
        SetWindow->setWindowTitle(QCoreApplication::translate("SetWindow", "\347\263\273\347\273\237\350\256\276\347\275\256", nullptr));
        btnNetwork->setText(QCoreApplication::translate("SetWindow", "\347\275\221\347\273\234\350\277\236\346\216\245", nullptr));
        btnFace->setText(QCoreApplication::translate("SetWindow", "\344\272\272\350\204\270\350\257\206\345\210\253", nullptr));
        btnAttendance->setText(QCoreApplication::translate("SetWindow", "\350\200\203\345\213\244\350\247\204\345\210\231", nullptr));
        btnStorage->setText(QCoreApplication::translate("SetWindow", "\345\255\230\345\202\250\350\256\276\347\275\256", nullptr));
        networkGroup->setTitle(QCoreApplication::translate("SetWindow", "\347\275\221\347\273\234\350\277\236\346\216\245\350\256\276\347\275\256", nullptr));
        ipLabel->setText(QCoreApplication::translate("SetWindow", "\346\234\215\345\212\241\345\231\250\345\234\260\345\235\200", nullptr));
        lineEditIP->setPlaceholderText(QCoreApplication::translate("SetWindow", "\344\276\213\345\246\202: 192.168.1.100", nullptr));
        portLabel->setText(QCoreApplication::translate("SetWindow", "\347\253\257\345\217\243", nullptr));
        timeoutLabel->setText(QCoreApplication::translate("SetWindow", "\350\277\236\346\216\245\350\266\205\346\227\266", nullptr));
        spinBoxTimeout->setSuffix(QCoreApplication::translate("SetWindow", " \347\247\222", nullptr));
        btnTestConnection->setText(QCoreApplication::translate("SetWindow", "\346\265\213\350\257\225\350\277\236\346\216\245", nullptr));
        labelConnectionStatus->setText(QCoreApplication::translate("SetWindow", "\346\234\252\350\277\236\346\216\245", nullptr));
        faceGroup->setTitle(QCoreApplication::translate("SetWindow", "\344\272\272\350\204\270\350\257\206\345\210\253\350\256\276\347\275\256", nullptr));
        thresholdLabel->setText(QCoreApplication::translate("SetWindow", "\347\233\270\344\274\274\345\272\246\351\230\210\345\200\274", nullptr));
        labelThresholdValue->setText(QCoreApplication::translate("SetWindow", "80%", nullptr));
        maxFaceLabel->setText(QCoreApplication::translate("SetWindow", "\346\234\200\345\244\247\346\243\200\346\265\213\344\272\272\350\204\270\346\225\260", nullptr));
        recognizeTimeoutLabel->setText(QCoreApplication::translate("SetWindow", "\350\257\206\345\210\253\350\266\205\346\227\266\346\227\266\351\227\264", nullptr));
        spinBoxRecognizeTimeout->setSuffix(QCoreApplication::translate("SetWindow", " \347\247\222", nullptr));
        attendanceGroup->setTitle(QCoreApplication::translate("SetWindow", "\350\200\203\345\213\244\350\247\204\345\210\231\350\256\276\347\275\256", nullptr));
        workStartLabel->setText(QCoreApplication::translate("SetWindow", "\344\270\212\347\217\255\346\227\266\351\227\264", nullptr));
        timeEditWorkStart->setDisplayFormat(QCoreApplication::translate("SetWindow", "HH:mm", nullptr));
        workEndLabel->setText(QCoreApplication::translate("SetWindow", "\344\270\213\347\217\255\346\227\266\351\227\264", nullptr));
        timeEditWorkEnd->setDisplayFormat(QCoreApplication::translate("SetWindow", "HH:mm", nullptr));
        lateLabel->setText(QCoreApplication::translate("SetWindow", "\350\277\237\345\210\260\345\205\201\350\256\270", nullptr));
        spinBoxLate->setSuffix(QCoreApplication::translate("SetWindow", " \345\210\206\351\222\237", nullptr));
        earlyLabel->setText(QCoreApplication::translate("SetWindow", "\346\227\251\351\200\200\345\205\201\350\256\270", nullptr));
        spinBoxEarly->setSuffix(QCoreApplication::translate("SetWindow", " \345\210\206\351\222\237", nullptr));
        storageGroup->setTitle(QCoreApplication::translate("SetWindow", "\345\255\230\345\202\250\350\256\276\347\275\256", nullptr));
        dbPathLabel->setText(QCoreApplication::translate("SetWindow", "\346\225\260\346\215\256\345\272\223\350\267\257\345\276\204", nullptr));
        lineEditDbPath->setPlaceholderText(QCoreApplication::translate("SetWindow", "\350\257\267\351\200\211\346\213\251\346\225\260\346\215\256\345\272\223\346\226\207\344\273\266\350\267\257\345\276\204", nullptr));
        btnBrowseDb->setText(QCoreApplication::translate("SetWindow", "\346\265\217\350\247\210...", nullptr));
        logPathLabel->setText(QCoreApplication::translate("SetWindow", "\346\227\245\345\277\227\350\267\257\345\276\204", nullptr));
        lineEditLogPath->setPlaceholderText(QCoreApplication::translate("SetWindow", "\350\257\267\351\200\211\346\213\251\346\227\245\345\277\227\346\226\207\344\273\266\344\277\235\345\255\230\350\267\257\345\276\204", nullptr));
        btnBrowseLog->setText(QCoreApplication::translate("SetWindow", "\346\265\217\350\247\210...", nullptr));
        cachePathLabel->setText(QCoreApplication::translate("SetWindow", "\347\274\223\345\255\230\347\233\256\345\275\225", nullptr));
        lineEditCachePath->setPlaceholderText(QCoreApplication::translate("SetWindow", "\350\257\267\351\200\211\346\213\251\345\233\276\347\211\207\347\274\223\345\255\230\347\233\256\345\275\225", nullptr));
        btnBrowseCache->setText(QCoreApplication::translate("SetWindow", "\346\265\217\350\247\210...", nullptr));
        btnRestore->setText(QCoreApplication::translate("SetWindow", "\346\201\242\345\244\215\351\273\230\350\256\244", nullptr));
        btnCancel->setText(QCoreApplication::translate("SetWindow", "\345\217\226\346\266\210", nullptr));
        btnSave->setText(QCoreApplication::translate("SetWindow", "\344\277\235\345\255\230\350\256\276\347\275\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SetWindow: public Ui_SetWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETWINDOW_H
