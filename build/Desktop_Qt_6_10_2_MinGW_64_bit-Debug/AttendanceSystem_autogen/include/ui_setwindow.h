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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
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
    QPushButton *btnStorage;
    QPushButton *btnDevice;
    QPushButton *btnSync;
    QSpacerItem *navSpacer;
    QWidget *contentWidget;
    QVBoxLayout *contentLayout;
    QStackedWidget *stackedWidget;
    QWidget *pageNetwork;
    QVBoxLayout *networkPageLayout;
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
    QPushButton *btnDisconnect;
    QLabel *labelConnectionStatus;
    QSpacerItem *testSpacer;
    QSpacerItem *networkSpacer;
    QWidget *pageFace;
    QVBoxLayout *facePageLayout;
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
    QGroupBox *arcfaceGroup;
    QVBoxLayout *arcfaceLayout;
    QHBoxLayout *appIdLayout;
    QLabel *appIdLabel;
    QLineEdit *lineEditAppId;
    QHBoxLayout *sdkKeyLayout;
    QLabel *sdkKeyLabel;
    QLineEdit *lineEditSdkKey;
    QGroupBox *rotationGroup;
    QVBoxLayout *rotationGroupLayout;
    QHBoxLayout *rotationLayout;
    QLabel *labelRotation;
    QLineEdit *lineEditRotation;
    QPushButton *btnRotate;
    QSpacerItem *rotationSpacer;
    QLabel *labelRotationHint;
    QSpacerItem *faceSpacer;
    QWidget *pageStorage;
    QVBoxLayout *storagePageLayout;
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
    QSpacerItem *storageSpacer;
    QWidget *pageDevice;
    QVBoxLayout *devicePageLayout;
    QGroupBox *deviceGroup;
    QVBoxLayout *deviceLayout;
    QHBoxLayout *deviceIdLayout;
    QLabel *labelDeviceId;
    QLineEdit *lineEditDeviceId;
    QHBoxLayout *deviceKeyLayout;
    QLabel *labelDeviceKey;
    QLineEdit *lineEditDeviceKey;
    QHBoxLayout *fwVersionLayout;
    QLabel *labelFwVersion;
    QLineEdit *lineEditFwVersion;
    QHBoxLayout *deviceNameLayout;
    QLabel *labelDeviceName;
    QLineEdit *lineEditDeviceName;
    QSpacerItem *deviceSpacer;
    QWidget *pageSync;
    QVBoxLayout *syncPageLayout;
    QGroupBox *syncGroup;
    QVBoxLayout *syncLayout;
    QCheckBox *checkBoxAutoSync;
    QHBoxLayout *syncTimeoutLayout;
    QLabel *labelSyncTimeout;
    QSpinBox *spinBoxSyncTimeout;
    QSpacerItem *syncTimeoutSpacer;
    QSpacerItem *syncSpacer;
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

        btnStorage = new QPushButton(navWidget);
        btnStorage->setObjectName("btnStorage");
        btnStorage->setMinimumSize(QSize(0, 45));
        btnStorage->setCheckable(true);
        btnStorage->setAutoExclusive(true);

        navLayout->addWidget(btnStorage);

        btnDevice = new QPushButton(navWidget);
        btnDevice->setObjectName("btnDevice");
        btnDevice->setMinimumSize(QSize(0, 45));
        btnDevice->setCheckable(true);
        btnDevice->setAutoExclusive(true);

        navLayout->addWidget(btnDevice);

        btnSync = new QPushButton(navWidget);
        btnSync->setObjectName("btnSync");
        btnSync->setMinimumSize(QSize(0, 45));
        btnSync->setCheckable(true);
        btnSync->setAutoExclusive(true);

        navLayout->addWidget(btnSync);

        navSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        navLayout->addItem(navSpacer);


        mainLayout->addWidget(navWidget);

        contentWidget = new QWidget(SetWindow);
        contentWidget->setObjectName("contentWidget");
        contentLayout = new QVBoxLayout(contentWidget);
        contentLayout->setSpacing(15);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(30, 20, 30, 20);
        stackedWidget = new QStackedWidget(contentWidget);
        stackedWidget->setObjectName("stackedWidget");
        pageNetwork = new QWidget();
        pageNetwork->setObjectName("pageNetwork");
        networkPageLayout = new QVBoxLayout(pageNetwork);
        networkPageLayout->setSpacing(20);
        networkPageLayout->setObjectName("networkPageLayout");
        networkPageLayout->setContentsMargins(0, 0, 0, 0);
        networkGroup = new QGroupBox(pageNetwork);
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

        btnDisconnect = new QPushButton(networkGroup);
        btnDisconnect->setObjectName("btnDisconnect");
        btnDisconnect->setEnabled(false);

        testLayout->addWidget(btnDisconnect);

        labelConnectionStatus = new QLabel(networkGroup);
        labelConnectionStatus->setObjectName("labelConnectionStatus");
        labelConnectionStatus->setStyleSheet(QString::fromUtf8("color: red; font-weight: bold;"));

        testLayout->addWidget(labelConnectionStatus);

        testSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        testLayout->addItem(testSpacer);


        networkLayout->addLayout(testLayout);


        networkPageLayout->addWidget(networkGroup);

        networkSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        networkPageLayout->addItem(networkSpacer);

        stackedWidget->addWidget(pageNetwork);
        pageFace = new QWidget();
        pageFace->setObjectName("pageFace");
        facePageLayout = new QVBoxLayout(pageFace);
        facePageLayout->setSpacing(20);
        facePageLayout->setObjectName("facePageLayout");
        facePageLayout->setContentsMargins(0, 0, 0, 0);
        faceGroup = new QGroupBox(pageFace);
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


        facePageLayout->addWidget(faceGroup);

        arcfaceGroup = new QGroupBox(pageFace);
        arcfaceGroup->setObjectName("arcfaceGroup");
        arcfaceLayout = new QVBoxLayout(arcfaceGroup);
        arcfaceLayout->setSpacing(15);
        arcfaceLayout->setObjectName("arcfaceLayout");
        appIdLayout = new QHBoxLayout();
        appIdLayout->setObjectName("appIdLayout");
        appIdLabel = new QLabel(arcfaceGroup);
        appIdLabel->setObjectName("appIdLabel");
        appIdLabel->setMinimumSize(QSize(120, 0));

        appIdLayout->addWidget(appIdLabel);

        lineEditAppId = new QLineEdit(arcfaceGroup);
        lineEditAppId->setObjectName("lineEditAppId");

        appIdLayout->addWidget(lineEditAppId);


        arcfaceLayout->addLayout(appIdLayout);

        sdkKeyLayout = new QHBoxLayout();
        sdkKeyLayout->setObjectName("sdkKeyLayout");
        sdkKeyLabel = new QLabel(arcfaceGroup);
        sdkKeyLabel->setObjectName("sdkKeyLabel");
        sdkKeyLabel->setMinimumSize(QSize(120, 0));

        sdkKeyLayout->addWidget(sdkKeyLabel);

        lineEditSdkKey = new QLineEdit(arcfaceGroup);
        lineEditSdkKey->setObjectName("lineEditSdkKey");

        sdkKeyLayout->addWidget(lineEditSdkKey);


        arcfaceLayout->addLayout(sdkKeyLayout);


        facePageLayout->addWidget(arcfaceGroup);

        rotationGroup = new QGroupBox(pageFace);
        rotationGroup->setObjectName("rotationGroup");
        rotationGroupLayout = new QVBoxLayout(rotationGroup);
        rotationGroupLayout->setSpacing(10);
        rotationGroupLayout->setObjectName("rotationGroupLayout");
        rotationLayout = new QHBoxLayout();
        rotationLayout->setObjectName("rotationLayout");
        labelRotation = new QLabel(rotationGroup);
        labelRotation->setObjectName("labelRotation");
        labelRotation->setMinimumSize(QSize(120, 0));

        rotationLayout->addWidget(labelRotation);

        lineEditRotation = new QLineEdit(rotationGroup);
        lineEditRotation->setObjectName("lineEditRotation");
        lineEditRotation->setReadOnly(true);
        lineEditRotation->setMaximumSize(QSize(80, 16777215));
        lineEditRotation->setAlignment(Qt::AlignmentFlag::AlignCenter);

        rotationLayout->addWidget(lineEditRotation);

        btnRotate = new QPushButton(rotationGroup);
        btnRotate->setObjectName("btnRotate");
        btnRotate->setMinimumSize(QSize(100, 0));

        rotationLayout->addWidget(btnRotate);

        rotationSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        rotationLayout->addItem(rotationSpacer);


        rotationGroupLayout->addLayout(rotationLayout);

        labelRotationHint = new QLabel(rotationGroup);
        labelRotationHint->setObjectName("labelRotationHint");
        labelRotationHint->setStyleSheet(QString::fromUtf8("color: #888888; font-size: 11px;"));

        rotationGroupLayout->addWidget(labelRotationHint);


        facePageLayout->addWidget(rotationGroup);

        faceSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        facePageLayout->addItem(faceSpacer);

        stackedWidget->addWidget(pageFace);
        pageStorage = new QWidget();
        pageStorage->setObjectName("pageStorage");
        storagePageLayout = new QVBoxLayout(pageStorage);
        storagePageLayout->setSpacing(20);
        storagePageLayout->setObjectName("storagePageLayout");
        storagePageLayout->setContentsMargins(0, 0, 0, 0);
        storageGroup = new QGroupBox(pageStorage);
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


        storagePageLayout->addWidget(storageGroup);

        storageSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        storagePageLayout->addItem(storageSpacer);

        stackedWidget->addWidget(pageStorage);
        pageDevice = new QWidget();
        pageDevice->setObjectName("pageDevice");
        devicePageLayout = new QVBoxLayout(pageDevice);
        devicePageLayout->setSpacing(20);
        devicePageLayout->setObjectName("devicePageLayout");
        devicePageLayout->setContentsMargins(0, 0, 0, 0);
        deviceGroup = new QGroupBox(pageDevice);
        deviceGroup->setObjectName("deviceGroup");
        deviceLayout = new QVBoxLayout(deviceGroup);
        deviceLayout->setSpacing(15);
        deviceLayout->setObjectName("deviceLayout");
        deviceIdLayout = new QHBoxLayout();
        deviceIdLayout->setObjectName("deviceIdLayout");
        labelDeviceId = new QLabel(deviceGroup);
        labelDeviceId->setObjectName("labelDeviceId");
        labelDeviceId->setMinimumSize(QSize(100, 0));

        deviceIdLayout->addWidget(labelDeviceId);

        lineEditDeviceId = new QLineEdit(deviceGroup);
        lineEditDeviceId->setObjectName("lineEditDeviceId");

        deviceIdLayout->addWidget(lineEditDeviceId);


        deviceLayout->addLayout(deviceIdLayout);

        deviceKeyLayout = new QHBoxLayout();
        deviceKeyLayout->setObjectName("deviceKeyLayout");
        labelDeviceKey = new QLabel(deviceGroup);
        labelDeviceKey->setObjectName("labelDeviceKey");
        labelDeviceKey->setMinimumSize(QSize(100, 0));

        deviceKeyLayout->addWidget(labelDeviceKey);

        lineEditDeviceKey = new QLineEdit(deviceGroup);
        lineEditDeviceKey->setObjectName("lineEditDeviceKey");
        lineEditDeviceKey->setEchoMode(QLineEdit::EchoMode::Password);

        deviceKeyLayout->addWidget(lineEditDeviceKey);


        deviceLayout->addLayout(deviceKeyLayout);

        fwVersionLayout = new QHBoxLayout();
        fwVersionLayout->setObjectName("fwVersionLayout");
        labelFwVersion = new QLabel(deviceGroup);
        labelFwVersion->setObjectName("labelFwVersion");
        labelFwVersion->setMinimumSize(QSize(100, 0));

        fwVersionLayout->addWidget(labelFwVersion);

        lineEditFwVersion = new QLineEdit(deviceGroup);
        lineEditFwVersion->setObjectName("lineEditFwVersion");
        lineEditFwVersion->setReadOnly(true);

        fwVersionLayout->addWidget(lineEditFwVersion);


        deviceLayout->addLayout(fwVersionLayout);

        deviceNameLayout = new QHBoxLayout();
        deviceNameLayout->setObjectName("deviceNameLayout");
        labelDeviceName = new QLabel(deviceGroup);
        labelDeviceName->setObjectName("labelDeviceName");
        labelDeviceName->setMinimumSize(QSize(100, 0));

        deviceNameLayout->addWidget(labelDeviceName);

        lineEditDeviceName = new QLineEdit(deviceGroup);
        lineEditDeviceName->setObjectName("lineEditDeviceName");

        deviceNameLayout->addWidget(lineEditDeviceName);


        deviceLayout->addLayout(deviceNameLayout);


        devicePageLayout->addWidget(deviceGroup);

        deviceSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        devicePageLayout->addItem(deviceSpacer);

        stackedWidget->addWidget(pageDevice);
        pageSync = new QWidget();
        pageSync->setObjectName("pageSync");
        syncPageLayout = new QVBoxLayout(pageSync);
        syncPageLayout->setSpacing(20);
        syncPageLayout->setObjectName("syncPageLayout");
        syncPageLayout->setContentsMargins(0, 0, 0, 0);
        syncGroup = new QGroupBox(pageSync);
        syncGroup->setObjectName("syncGroup");
        syncLayout = new QVBoxLayout(syncGroup);
        syncLayout->setSpacing(15);
        syncLayout->setObjectName("syncLayout");
        checkBoxAutoSync = new QCheckBox(syncGroup);
        checkBoxAutoSync->setObjectName("checkBoxAutoSync");
        checkBoxAutoSync->setChecked(true);

        syncLayout->addWidget(checkBoxAutoSync);

        syncTimeoutLayout = new QHBoxLayout();
        syncTimeoutLayout->setObjectName("syncTimeoutLayout");
        labelSyncTimeout = new QLabel(syncGroup);
        labelSyncTimeout->setObjectName("labelSyncTimeout");
        labelSyncTimeout->setMinimumSize(QSize(100, 0));

        syncTimeoutLayout->addWidget(labelSyncTimeout);

        spinBoxSyncTimeout = new QSpinBox(syncGroup);
        spinBoxSyncTimeout->setObjectName("spinBoxSyncTimeout");
        spinBoxSyncTimeout->setMinimum(60);
        spinBoxSyncTimeout->setMaximum(600);
        spinBoxSyncTimeout->setValue(300);

        syncTimeoutLayout->addWidget(spinBoxSyncTimeout);

        syncTimeoutSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        syncTimeoutLayout->addItem(syncTimeoutSpacer);


        syncLayout->addLayout(syncTimeoutLayout);


        syncPageLayout->addWidget(syncGroup);

        syncSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        syncPageLayout->addItem(syncSpacer);

        stackedWidget->addWidget(pageSync);

        contentLayout->addWidget(stackedWidget);

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

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SetWindow);
    } // setupUi

    void retranslateUi(QWidget *SetWindow)
    {
        SetWindow->setWindowTitle(QCoreApplication::translate("SetWindow", "\347\263\273\347\273\237\350\256\276\347\275\256", nullptr));
        btnNetwork->setText(QCoreApplication::translate("SetWindow", "\347\275\221\347\273\234\350\277\236\346\216\245", nullptr));
        btnFace->setText(QCoreApplication::translate("SetWindow", "\344\272\272\350\204\270\350\257\206\345\210\253", nullptr));
        btnStorage->setText(QCoreApplication::translate("SetWindow", "\345\255\230\345\202\250\350\256\276\347\275\256", nullptr));
        btnDevice->setText(QCoreApplication::translate("SetWindow", "\350\256\276\345\244\207\344\277\241\346\201\257", nullptr));
        btnSync->setText(QCoreApplication::translate("SetWindow", "\345\220\214\346\255\245\350\256\276\347\275\256", nullptr));
        networkGroup->setTitle(QCoreApplication::translate("SetWindow", "\347\275\221\347\273\234\350\277\236\346\216\245\350\256\276\347\275\256", nullptr));
        ipLabel->setText(QCoreApplication::translate("SetWindow", "\346\234\215\345\212\241\345\231\250\345\234\260\345\235\200", nullptr));
        lineEditIP->setPlaceholderText(QCoreApplication::translate("SetWindow", "\344\276\213\345\246\202: 192.168.1.100", nullptr));
        portLabel->setText(QCoreApplication::translate("SetWindow", "\347\253\257\345\217\243", nullptr));
        timeoutLabel->setText(QCoreApplication::translate("SetWindow", "\350\277\236\346\216\245\350\266\205\346\227\266", nullptr));
        spinBoxTimeout->setSuffix(QCoreApplication::translate("SetWindow", " \347\247\222", nullptr));
        btnTestConnection->setText(QCoreApplication::translate("SetWindow", "\350\277\236\346\216\245\346\234\215\345\212\241\345\231\250", nullptr));
        btnDisconnect->setText(QCoreApplication::translate("SetWindow", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
        labelConnectionStatus->setText(QCoreApplication::translate("SetWindow", "\346\234\252\350\277\236\346\216\245", nullptr));
        faceGroup->setTitle(QCoreApplication::translate("SetWindow", "\344\272\272\350\204\270\350\257\206\345\210\253\350\256\276\347\275\256", nullptr));
        thresholdLabel->setText(QCoreApplication::translate("SetWindow", "\347\233\270\344\274\274\345\272\246\351\230\210\345\200\274", nullptr));
        labelThresholdValue->setText(QCoreApplication::translate("SetWindow", "80%", nullptr));
        maxFaceLabel->setText(QCoreApplication::translate("SetWindow", "\346\234\200\345\244\247\346\243\200\346\265\213\344\272\272\350\204\270\346\225\260", nullptr));
        recognizeTimeoutLabel->setText(QCoreApplication::translate("SetWindow", "\350\257\206\345\210\253\350\266\205\346\227\266\346\227\266\351\227\264", nullptr));
        spinBoxRecognizeTimeout->setSuffix(QCoreApplication::translate("SetWindow", " \347\247\222", nullptr));
        arcfaceGroup->setTitle(QCoreApplication::translate("SetWindow", "ArcFace SDK\351\205\215\347\275\256", nullptr));
        appIdLabel->setText(QCoreApplication::translate("SetWindow", "App ID", nullptr));
        lineEditAppId->setPlaceholderText(QCoreApplication::translate("SetWindow", "\350\257\267\350\276\223\345\205\245ArcFace App ID", nullptr));
        sdkKeyLabel->setText(QCoreApplication::translate("SetWindow", "SDK Key", nullptr));
        lineEditSdkKey->setPlaceholderText(QCoreApplication::translate("SetWindow", "\350\257\267\350\276\223\345\205\245ArcFace SDK Key", nullptr));
        rotationGroup->setTitle(QCoreApplication::translate("SetWindow", "\346\221\204\345\203\217\345\244\264\346\226\271\345\220\221", nullptr));
        labelRotation->setText(QCoreApplication::translate("SetWindow", "\345\275\223\345\211\215\350\247\222\345\272\246", nullptr));
        btnRotate->setText(QCoreApplication::translate("SetWindow", "\346\227\213\350\275\254 90\302\260", nullptr));
        labelRotationHint->setText(QCoreApplication::translate("SetWindow", "\346\257\217\346\254\241\347\202\271\345\207\273\346\227\213\350\275\254 90\302\260\357\274\2100\302\260 \342\206\222 90\302\260 \342\206\222 180\302\260 \342\206\222 270\302\260 \345\276\252\347\216\257\357\274\211", nullptr));
        storageGroup->setTitle(QCoreApplication::translate("SetWindow", "\345\255\230\345\202\250\350\256\276\347\275\256", nullptr));
        dbPathLabel->setText(QCoreApplication::translate("SetWindow", "\346\225\260\346\215\256\345\272\223\350\267\257\345\276\204", nullptr));
        lineEditDbPath->setPlaceholderText(QCoreApplication::translate("SetWindow", "\350\257\267\351\200\211\346\213\251\346\225\260\346\215\256\345\272\223\346\226\207\344\273\266\350\267\257\345\276\204", nullptr));
        btnBrowseDb->setText(QCoreApplication::translate("SetWindow", "\346\265\217\350\247\210...", nullptr));
        logPathLabel->setText(QCoreApplication::translate("SetWindow", "\346\227\245\345\277\227\350\267\257\345\276\204", nullptr));
        lineEditLogPath->setPlaceholderText(QCoreApplication::translate("SetWindow", "\350\257\267\351\200\211\346\213\251\346\227\245\345\277\227\346\226\207\344\273\266\344\277\235\345\255\230\350\267\257\345\276\204", nullptr));
        btnBrowseLog->setText(QCoreApplication::translate("SetWindow", "\346\265\217\350\247\210...", nullptr));
        deviceGroup->setTitle(QCoreApplication::translate("SetWindow", "\350\256\276\345\244\207\344\277\241\346\201\257", nullptr));
        labelDeviceId->setText(QCoreApplication::translate("SetWindow", "\350\256\276\345\244\207 ID", nullptr));
        lineEditDeviceId->setPlaceholderText(QCoreApplication::translate("SetWindow", "\350\256\276\345\244\207\345\224\257\344\270\200\346\240\207\350\257\206", nullptr));
        labelDeviceKey->setText(QCoreApplication::translate("SetWindow", "\350\256\276\345\244\207\345\257\206\351\222\245", nullptr));
        lineEditDeviceKey->setPlaceholderText(QCoreApplication::translate("SetWindow", "\350\257\267\350\276\223\345\205\245\350\256\276\345\244\207\345\257\206\351\222\245", nullptr));
        labelFwVersion->setText(QCoreApplication::translate("SetWindow", "\345\233\272\344\273\266\347\211\210\346\234\254", nullptr));
        lineEditFwVersion->setPlaceholderText(QCoreApplication::translate("SetWindow", "\345\233\272\344\273\266\347\211\210\346\234\254\345\217\267", nullptr));
        labelDeviceName->setText(QCoreApplication::translate("SetWindow", "\350\256\276\345\244\207\345\220\215\347\247\260", nullptr));
        lineEditDeviceName->setPlaceholderText(QCoreApplication::translate("SetWindow", "\350\257\267\350\276\223\345\205\245\350\256\276\345\244\207\345\220\215\347\247\260", nullptr));
        syncGroup->setTitle(QCoreApplication::translate("SetWindow", "\345\220\214\346\255\245\350\256\276\347\275\256", nullptr));
        checkBoxAutoSync->setText(QCoreApplication::translate("SetWindow", "\350\277\236\346\216\245\345\220\216\350\207\252\345\212\250\345\220\214\346\255\245", nullptr));
        labelSyncTimeout->setText(QCoreApplication::translate("SetWindow", "\345\220\214\346\255\245\350\266\205\346\227\266", nullptr));
        spinBoxSyncTimeout->setSuffix(QCoreApplication::translate("SetWindow", " \347\247\222", nullptr));
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
