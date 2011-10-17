/********************************************************************************
** Form generated from reading UI file 'ComponentPropertyEditorPage.ui'
**
** Created: Mon 3. Oct 22:07:43 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPONENTPROPERTYEDITORPAGE_H
#define UI_COMPONENTPROPERTYEDITORPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QToolButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ComponentPropertyEditorPage
{
public:
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QVBoxLayout *vboxLayout1;
    QLabel *label_2;
    QTreeWidget *componentTree;
    QWidget *layoutWidget1;
    QVBoxLayout *vboxLayout2;
    QLabel *label_3;
    QHBoxLayout *hboxLayout;
    QToolButton *cmdAddProperty;
    QToolButton *cmdRemoveProperty;
    QSpacerItem *spacerItem;
    QTreeWidget *propertyEditor;

    void setupUi(QWidget *ComponentPropertyEditorPage)
    {
        if (ComponentPropertyEditorPage->objectName().isEmpty())
            ComponentPropertyEditorPage->setObjectName(QString::fromUtf8("ComponentPropertyEditorPage"));
        ComponentPropertyEditorPage->resize(504, 522);
        vboxLayout = new QVBoxLayout(ComponentPropertyEditorPage);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        label = new QLabel(ComponentPropertyEditorPage);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        vboxLayout->addWidget(label);

        splitter = new QSplitter(ComponentPropertyEditorPage);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy);
        splitter->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        vboxLayout1 = new QVBoxLayout(layoutWidget);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        vboxLayout1->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        vboxLayout1->addWidget(label_2);

        componentTree = new QTreeWidget(layoutWidget);
        componentTree->setObjectName(QString::fromUtf8("componentTree"));
        componentTree->setMaximumSize(QSize(16777215, 16777215));
        componentTree->setColumnCount(1);

        vboxLayout1->addWidget(componentTree);

        splitter->addWidget(layoutWidget);
        layoutWidget1 = new QWidget(splitter);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        vboxLayout2 = new QVBoxLayout(layoutWidget1);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        vboxLayout2->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(layoutWidget1);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        vboxLayout2->addWidget(label_3);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        cmdAddProperty = new QToolButton(layoutWidget1);
        cmdAddProperty->setObjectName(QString::fromUtf8("cmdAddProperty"));
        cmdAddProperty->setEnabled(false);
        cmdAddProperty->setMinimumSize(QSize(30, 30));

        hboxLayout->addWidget(cmdAddProperty);

        cmdRemoveProperty = new QToolButton(layoutWidget1);
        cmdRemoveProperty->setObjectName(QString::fromUtf8("cmdRemoveProperty"));
        cmdRemoveProperty->setEnabled(false);
        cmdRemoveProperty->setMinimumSize(QSize(30, 30));

        hboxLayout->addWidget(cmdRemoveProperty);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);


        vboxLayout2->addLayout(hboxLayout);

        propertyEditor = new QTreeWidget(layoutWidget1);
        propertyEditor->setObjectName(QString::fromUtf8("propertyEditor"));
        propertyEditor->setAlternatingRowColors(true);
        propertyEditor->setIndentation(0);
        propertyEditor->setAllColumnsShowFocus(true);
        propertyEditor->setColumnCount(2);

        vboxLayout2->addWidget(propertyEditor);

        splitter->addWidget(layoutWidget1);

        vboxLayout->addWidget(splitter);


        retranslateUi(ComponentPropertyEditorPage);

        QMetaObject::connectSlotsByName(ComponentPropertyEditorPage);
    } // setupUi

    void retranslateUi(QWidget *ComponentPropertyEditorPage)
    {
        ComponentPropertyEditorPage->setWindowTitle(QApplication::translate("ComponentPropertyEditorPage", "Component Properties", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ComponentPropertyEditorPage", "Configure the properties of objects exposed by different components. These properties will be saved into the component's XML file and will be used by the component the next time it loads.", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ComponentPropertyEditorPage", "Component Object Tree", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = componentTree->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("ComponentPropertyEditorPage", "1", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ComponentPropertyEditorPage", "Properties", 0, QApplication::UnicodeUTF8));
        cmdAddProperty->setText(QApplication::translate("ComponentPropertyEditorPage", "Add Property", 0, QApplication::UnicodeUTF8));
        cmdRemoveProperty->setText(QApplication::translate("ComponentPropertyEditorPage", "Remove Property", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem1 = propertyEditor->headerItem();
        ___qtreewidgetitem1->setText(1, QApplication::translate("ComponentPropertyEditorPage", "1", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem1->setText(0, QApplication::translate("ComponentPropertyEditorPage", "1", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ComponentPropertyEditorPage: public Ui_ComponentPropertyEditorPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPONENTPROPERTYEDITORPAGE_H
