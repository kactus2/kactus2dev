/********************************************************************************
** Form generated from reading UI file 'ConfigurationDialog.ui'
**
** Created: Wed 12. Oct 15:35:38 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGURATIONDIALOG_H
#define UI_CONFIGURATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigurationDialog
{
public:
    QGridLayout *gridLayout;
    QListWidget *lstComponents;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QWidget *page_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ConfigurationDialog)
    {
        if (ConfigurationDialog->objectName().isEmpty())
            ConfigurationDialog->setObjectName(QString::fromUtf8("ConfigurationDialog"));
        ConfigurationDialog->resize(759, 519);
        gridLayout = new QGridLayout(ConfigurationDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lstComponents = new QListWidget(ConfigurationDialog);
        lstComponents->setObjectName(QString::fromUtf8("lstComponents"));
        lstComponents->setMinimumSize(QSize(140, 0));
        lstComponents->setMaximumSize(QSize(140, 16777215));
        lstComponents->setSpacing(5);
        lstComponents->setGridSize(QSize(135, 50));
        lstComponents->setViewMode(QListView::IconMode);

        gridLayout->addWidget(lstComponents, 0, 0, 1, 1);

        stackedWidget = new QStackedWidget(ConfigurationDialog);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy);
        stackedWidget->setFrameShape(QFrame::Box);
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        page->setGeometry(QRect(1, 1, 593, 467));
        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        page_2->setGeometry(QRect(0, 0, 100, 30));
        stackedWidget->addWidget(page_2);

        gridLayout->addWidget(stackedWidget, 0, 1, 1, 1);

        buttonBox = new QDialogButtonBox(ConfigurationDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 2);


        retranslateUi(ConfigurationDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ConfigurationDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ConfigurationDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ConfigurationDialog);
    } // setupUi

    void retranslateUi(QDialog *ConfigurationDialog)
    {
        ConfigurationDialog->setWindowTitle(QApplication::translate("ConfigurationDialog", "Configuration Dialog Box", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ConfigurationDialog: public Ui_ConfigurationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGURATIONDIALOG_H
