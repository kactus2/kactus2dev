/********************************************************************************
** Form generated from reading UI file 'AddPropertyDialog.ui'
**
** Created: Wed 12. Oct 15:35:38 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDPROPERTYDIALOG_H
#define UI_ADDPROPERTYDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddPropertyDialog
{
public:
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QWidget *inputWidgetContainer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *AddPropertyDialog)
    {
        if (AddPropertyDialog->objectName().isEmpty())
            AddPropertyDialog->setObjectName(QString::fromUtf8("AddPropertyDialog"));
        AddPropertyDialog->resize(406, 83);
        vboxLayout = new QVBoxLayout(AddPropertyDialog);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        label = new QLabel(AddPropertyDialog);
        label->setObjectName(QString::fromUtf8("label"));

        vboxLayout->addWidget(label);

        inputWidgetContainer = new QWidget(AddPropertyDialog);
        inputWidgetContainer->setObjectName(QString::fromUtf8("inputWidgetContainer"));

        vboxLayout->addWidget(inputWidgetContainer);

        buttonBox = new QDialogButtonBox(AddPropertyDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(AddPropertyDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), AddPropertyDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), AddPropertyDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(AddPropertyDialog);
    } // setupUi

    void retranslateUi(QDialog *AddPropertyDialog)
    {
        AddPropertyDialog->setWindowTitle(QApplication::translate("AddPropertyDialog", "Add Property Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("AddPropertyDialog", "Choose a property to add", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AddPropertyDialog: public Ui_AddPropertyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDPROPERTYDIALOG_H
