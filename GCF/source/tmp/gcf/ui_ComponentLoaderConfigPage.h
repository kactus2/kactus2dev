/********************************************************************************
** Form generated from reading UI file 'ComponentLoaderConfigPage.ui'
**
** Created: Mon 3. Oct 22:07:43 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPONENTLOADERCONFIGPAGE_H
#define UI_COMPONENTLOADERCONFIGPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ComponentLoaderConfigPage
{
public:
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QListWidget *pathList;
    QHBoxLayout *hboxLayout;
    QPushButton *cmdAddPath;
    QPushButton *cmdRemovePath;
    QSpacerItem *spacerItem;

    void setupUi(QWidget *ComponentLoaderConfigPage)
    {
        if (ComponentLoaderConfigPage->objectName().isEmpty())
            ComponentLoaderConfigPage->setObjectName(QString::fromUtf8("ComponentLoaderConfigPage"));
        ComponentLoaderConfigPage->resize(419, 476);
        vboxLayout = new QVBoxLayout(ComponentLoaderConfigPage);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        label = new QLabel(ComponentLoaderConfigPage);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        vboxLayout->addWidget(label);

        pathList = new QListWidget(ComponentLoaderConfigPage);
        pathList->setObjectName(QString::fromUtf8("pathList"));

        vboxLayout->addWidget(pathList);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        cmdAddPath = new QPushButton(ComponentLoaderConfigPage);
        cmdAddPath->setObjectName(QString::fromUtf8("cmdAddPath"));

        hboxLayout->addWidget(cmdAddPath);

        cmdRemovePath = new QPushButton(ComponentLoaderConfigPage);
        cmdRemovePath->setObjectName(QString::fromUtf8("cmdRemovePath"));

        hboxLayout->addWidget(cmdRemovePath);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);


        vboxLayout->addLayout(hboxLayout);


        retranslateUi(ComponentLoaderConfigPage);

        QMetaObject::connectSlotsByName(ComponentLoaderConfigPage);
    } // setupUi

    void retranslateUi(QWidget *ComponentLoaderConfigPage)
    {
        ComponentLoaderConfigPage->setWindowTitle(QApplication::translate("ComponentLoaderConfigPage", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ComponentLoaderConfigPage", "The Component Loader looks for loadable components from the following directories. You can add and/or remove from this list to configure the way the Loader works.", 0, QApplication::UnicodeUTF8));
        cmdAddPath->setText(QApplication::translate("ComponentLoaderConfigPage", "&Add Path", 0, QApplication::UnicodeUTF8));
        cmdRemovePath->setText(QApplication::translate("ComponentLoaderConfigPage", "&Remove Path", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ComponentLoaderConfigPage: public Ui_ComponentLoaderConfigPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPONENTLOADERCONFIGPAGE_H
