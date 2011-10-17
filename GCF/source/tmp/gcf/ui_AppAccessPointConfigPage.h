/********************************************************************************
** Form generated from reading UI file 'AppAccessPointConfigPage.ui'
**
** Created: Wed 12. Oct 15:35:39 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APPACCESSPOINTCONFIGPAGE_H
#define UI_APPACCESSPOINTCONFIGPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AppAccessPointConfigPage
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QGridLayout *gridLayout1;
    QLabel *label_3;
    QLineEdit *txtAccessPointName;
    QLabel *label_2;
    QCheckBox *chkEnableAccess;
    QLabel *label_4;
    QLineEdit *txtAddress;
    QLabel *label_5;
    QLineEdit *txtPort;
    QGroupBox *groupBox;
    QGridLayout *gridLayout2;
    QPushButton *cmdAppsToMeRemove;
    QSpacerItem *spacerItem;
    QListWidget *lstAppsToMe;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout3;
    QPushButton *cmdMeToAppsAdd;
    QPushButton *cmdMeToAppsRemove;
    QSpacerItem *spacerItem1;
    QListWidget *lstMeToApps;

    void setupUi(QWidget *AppAccessPointConfigPage)
    {
        if (AppAccessPointConfigPage->objectName().isEmpty())
            AppAccessPointConfigPage->setObjectName(QString::fromUtf8("AppAccessPointConfigPage"));
        AppAccessPointConfigPage->resize(494, 396);
        gridLayout = new QGridLayout(AppAccessPointConfigPage);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(AppAccessPointConfigPage);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setPointSize(15);
        label->setFont(font);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        gridLayout1 = new QGridLayout();
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        label_3 = new QLabel(AppAccessPointConfigPage);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout1->addWidget(label_3, 0, 0, 1, 1);

        txtAccessPointName = new QLineEdit(AppAccessPointConfigPage);
        txtAccessPointName->setObjectName(QString::fromUtf8("txtAccessPointName"));

        gridLayout1->addWidget(txtAccessPointName, 0, 1, 1, 1);

        label_2 = new QLabel(AppAccessPointConfigPage);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout1->addWidget(label_2, 1, 0, 1, 1);

        chkEnableAccess = new QCheckBox(AppAccessPointConfigPage);
        chkEnableAccess->setObjectName(QString::fromUtf8("chkEnableAccess"));

        gridLayout1->addWidget(chkEnableAccess, 1, 1, 1, 1);

        label_4 = new QLabel(AppAccessPointConfigPage);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout1->addWidget(label_4, 2, 0, 1, 1);

        txtAddress = new QLineEdit(AppAccessPointConfigPage);
        txtAddress->setObjectName(QString::fromUtf8("txtAddress"));
        txtAddress->setReadOnly(true);

        gridLayout1->addWidget(txtAddress, 2, 1, 1, 1);

        label_5 = new QLabel(AppAccessPointConfigPage);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout1->addWidget(label_5, 3, 0, 1, 1);

        txtPort = new QLineEdit(AppAccessPointConfigPage);
        txtPort->setObjectName(QString::fromUtf8("txtPort"));
        txtPort->setReadOnly(true);

        gridLayout1->addWidget(txtPort, 3, 1, 1, 1);


        gridLayout->addLayout(gridLayout1, 1, 0, 1, 2);

        groupBox = new QGroupBox(AppAccessPointConfigPage);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout2 = new QGridLayout(groupBox);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        cmdAppsToMeRemove = new QPushButton(groupBox);
        cmdAppsToMeRemove->setObjectName(QString::fromUtf8("cmdAppsToMeRemove"));

        gridLayout2->addWidget(cmdAppsToMeRemove, 0, 0, 1, 1);

        spacerItem = new QSpacerItem(131, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacerItem, 0, 1, 1, 1);

        lstAppsToMe = new QListWidget(groupBox);
        lstAppsToMe->setObjectName(QString::fromUtf8("lstAppsToMe"));

        gridLayout2->addWidget(lstAppsToMe, 1, 0, 1, 2);


        gridLayout->addWidget(groupBox, 2, 0, 1, 1);

        groupBox_2 = new QGroupBox(AppAccessPointConfigPage);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout3 = new QGridLayout(groupBox_2);
        gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
        cmdMeToAppsAdd = new QPushButton(groupBox_2);
        cmdMeToAppsAdd->setObjectName(QString::fromUtf8("cmdMeToAppsAdd"));

        gridLayout3->addWidget(cmdMeToAppsAdd, 0, 0, 1, 1);

        cmdMeToAppsRemove = new QPushButton(groupBox_2);
        cmdMeToAppsRemove->setObjectName(QString::fromUtf8("cmdMeToAppsRemove"));

        gridLayout3->addWidget(cmdMeToAppsRemove, 0, 1, 1, 1);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout3->addItem(spacerItem1, 0, 2, 1, 1);

        lstMeToApps = new QListWidget(groupBox_2);
        lstMeToApps->setObjectName(QString::fromUtf8("lstMeToApps"));

        gridLayout3->addWidget(lstMeToApps, 1, 0, 1, 3);


        gridLayout->addWidget(groupBox_2, 2, 1, 1, 1);


        retranslateUi(AppAccessPointConfigPage);

        QMetaObject::connectSlotsByName(AppAccessPointConfigPage);
    } // setupUi

    void retranslateUi(QWidget *AppAccessPointConfigPage)
    {
        AppAccessPointConfigPage->setWindowTitle(QApplication::translate("AppAccessPointConfigPage", "Application Access", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("AppAccessPointConfigPage", "Inter-Process Communication Configuration", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("AppAccessPointConfigPage", "Access Point Name", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("AppAccessPointConfigPage", "Enable Access", 0, QApplication::UnicodeUTF8));
        chkEnableAccess->setText(QString());
        label_4->setText(QApplication::translate("AppAccessPointConfigPage", "Access Point Address", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("AppAccessPointConfigPage", "Access Point Port", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("AppAccessPointConfigPage", "Applications connected to me", 0, QApplication::UnicodeUTF8));
        cmdAppsToMeRemove->setText(QApplication::translate("AppAccessPointConfigPage", "&Remove", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("AppAccessPointConfigPage", "Applications I am connected to", 0, QApplication::UnicodeUTF8));
        cmdMeToAppsAdd->setText(QApplication::translate("AppAccessPointConfigPage", "&Add", 0, QApplication::UnicodeUTF8));
        cmdMeToAppsRemove->setText(QApplication::translate("AppAccessPointConfigPage", "&Remove", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AppAccessPointConfigPage: public Ui_AppAccessPointConfigPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APPACCESSPOINTCONFIGPAGE_H
