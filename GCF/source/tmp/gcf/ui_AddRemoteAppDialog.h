/********************************************************************************
** Form generated from reading UI file 'AddRemoteAppDialog.ui'
**
** Created: Wed 12. Oct 15:35:39 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDREMOTEAPPDIALOG_H
#define UI_ADDREMOTEAPPDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_AddRemoteAppDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *txtIPAddress;
    QLabel *label_2;
    QLineEdit *txtPort;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *AddRemoteAppDialog)
    {
        if (AddRemoteAppDialog->objectName().isEmpty())
            AddRemoteAppDialog->setObjectName(QString::fromUtf8("AddRemoteAppDialog"));
        AddRemoteAppDialog->resize(350, 96);
        gridLayout = new QGridLayout(AddRemoteAppDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(AddRemoteAppDialog);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        txtIPAddress = new QLineEdit(AddRemoteAppDialog);
        txtIPAddress->setObjectName(QString::fromUtf8("txtIPAddress"));

        gridLayout->addWidget(txtIPAddress, 0, 1, 1, 1);

        label_2 = new QLabel(AddRemoteAppDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        txtPort = new QLineEdit(AddRemoteAppDialog);
        txtPort->setObjectName(QString::fromUtf8("txtPort"));

        gridLayout->addWidget(txtPort, 1, 1, 1, 1);

        buttonBox = new QDialogButtonBox(AddRemoteAppDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 0, 1, 2);


        retranslateUi(AddRemoteAppDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), AddRemoteAppDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), AddRemoteAppDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(AddRemoteAppDialog);
    } // setupUi

    void retranslateUi(QDialog *AddRemoteAppDialog)
    {
        AddRemoteAppDialog->setWindowTitle(QApplication::translate("AddRemoteAppDialog", "Connect to Application", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("AddRemoteAppDialog", "IP Address", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("AddRemoteAppDialog", "Port Number", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AddRemoteAppDialog: public Ui_AddRemoteAppDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDREMOTEAPPDIALOG_H
