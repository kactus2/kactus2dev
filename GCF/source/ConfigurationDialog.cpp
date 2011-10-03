	/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <GCF/ConfigurationDialog.h>
#include <GCF/ComponentFactory.h>
#include <GCF/AbstractComponent.h>
#include <GCF/ComponentGui.h>
#include "ui_ConfigurationDialog.h"
#include "ui_ComponentPropertyEditorPage.h"
#include "ui_AddPropertyDialog.h"

#include <QDomDocument>
#include <QDomElement>
#include <QStyle>
#include <QApplication>

#include <QMap>
#include <QMenu>
#include <QFile>
#include <QPainter>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QMetaProperty>
#include <QTreeWidgetItem>
#include <QAbstractItemDelegate>
#include <QStyleOptionViewItem>

struct GCF::ConfigurationDialogData
{
    Ui::ConfigurationDialog ui;
    GCF::ComponentPropertyEditorPage* propEditorPage;
    QWidget* firstWidget;
};

GCF::ConfigurationDialog::ConfigurationDialog(QWidget* parent)
:QDialog(parent)
{
    d = new GCF::ConfigurationDialogData;
    d->firstWidget = 0;
    d->ui.setupUi(this);

    // Properties page.
    d->propEditorPage = new GCF::ComponentPropertyEditorPage;
    addPage(d->propEditorPage);
    connect(okButton(), SIGNAL(clicked()), d->propEditorPage, SLOT(saveProperties()));
}

GCF::ConfigurationDialog::~ConfigurationDialog()
{
    delete d;
}

void GCF::ConfigurationDialog::addPage(QWidget* child)
{
    if(!child)
        return;

    QString title = child->windowTitle();
    QIcon icon = child->windowIcon();
    if(icon.isNull())
        icon = QIcon(":/GCF/configure.png");

    QListWidgetItem* item = new QListWidgetItem(icon, title, d->ui.lstComponents);
    item->setData(Qt::UserRole, qVariantFromValue<QWidget*>(child));
    child->setAutoFillBackground(true);
    d->ui.stackedWidget->addWidget(child);
    if(!d->firstWidget)
        d->firstWidget = child;
    if(d->firstWidget != child)
        child->hide();
    d->ui.stackedWidget->setCurrentWidget(d->firstWidget);
}

QWidget* GCF::ConfigurationDialog::pageContainer() const
{
    return d->ui.stackedWidget;
}

QPushButton* GCF::ConfigurationDialog::okButton() const
{
    return d->ui.buttonBox->button(QDialogButtonBox::Ok);
}

QPushButton* GCF::ConfigurationDialog::cancelButton() const
{
    return d->ui.buttonBox->button(QDialogButtonBox::Cancel);
}

void GCF::ConfigurationDialog::showEvent(QShowEvent* se)
{
    QMetaObject::invokeMethod(d->propEditorPage, "loadProperties", Qt::QueuedConnection);
    QDialog::showEvent(se);
}

void GCF::ConfigurationDialog::on_lstComponents_currentRowChanged(int index)
{
    QListWidgetItem* item = d->ui.lstComponents->item(index);
    if(!item)
        return;

    QWidget* widget = item->data(Qt::UserRole).value<QWidget*>();
    if(!widget)
        return;

    d->ui.stackedWidget->setCurrentWidget(widget);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::ComponentPropertyEditorPage
////////////////////////////////////////////////////////////////////////////////////////////////////

struct GCF::ComponentPropertyEditorPageData
{
    Ui::ComponentPropertyEditorPage ui;

    QMap< QString, QMap<QString, QString> > propertyMap;
    QString currentObject;
    GCF::ComponentGui currentGui;
    QMap<QString, QDomDocument> guiXmlMap;

    void loadComponentTree(GCF::AbstractComponent* comp);
    void saveProperties();
    void saveProperties(GCF::ComponentGuiNode node, const QMap<QString, QString> & propMap);
    void showProperties(const QString& completeName);
    QComboBox* createComboBox(const QString& completeName, QWidget* parent);
    QObject* getObject(const QString& completeName);
};

GCF::ComponentPropertyEditorPage::ComponentPropertyEditorPage(QWidget* parent)
:QWidget(parent)
{
    d = new GCF::ComponentPropertyEditorPageData;
    d->ui.setupUi(this);
    setWindowIcon(QIcon(":/GCF/configure.png"));

    d->ui.componentTree->setHeaderLabel("Component/Object");
    QStringList colNames = (QStringList() << "Property" << "Value");
    d->ui.propertyEditor->setHeaderLabels(colNames);
}

GCF::ComponentPropertyEditorPage::~ComponentPropertyEditorPage()
{
    delete d;
}

void GCF::ComponentPropertyEditorPage::loadProperties()
{
    if(d->ui.componentTree->topLevelItemCount())
        return;

    // First Load the components and their objects/widgets
    QList<GCF::AbstractComponent*> comps = GCF::ComponentFactory::instance().components();
    for(int i=0; i<comps.count(); i++)
    {
        GCF::AbstractComponent* comp = comps[i];
        d->loadComponentTree(comp);
    }
}

void GCF::ComponentPropertyEditorPage::saveProperties()
{
    // First Load the components and their objects/widgets
    d->saveProperties();

    QMessageBox::information(this, "Notification",
        "The changes will be applied when you restart the application");
}

void GCF::ComponentPropertyEditorPage::on_componentTree_itemClicked(QTreeWidgetItem* item)
{
    d->ui.propertyEditor->clear();
    d->currentObject.clear();
    d->currentGui = GCF::ComponentGui();

    if(!item->parent() || !item)
    {
        d->ui.cmdAddProperty->setEnabled(false);
        d->ui.cmdRemoveProperty->setEnabled(false);
        return;
    }

    d->currentObject = item->data(0, Qt::UserRole).toString();
    d->currentGui = item->parent()->data(0, Qt::UserRole).value<GCF::ComponentGui>();
    d->showProperties(d->currentObject);
    d->ui.cmdAddProperty->setEnabled(true);
    d->ui.cmdRemoveProperty->setEnabled(true);
}

void GCF::ComponentPropertyEditorPage::on_cmdAddProperty_clicked()
{
    if(d->currentObject.isEmpty())
        return;

    QDialog newPropDialog(this);
    Ui::AddPropertyDialog ui;
    ui.setupUi(&newPropDialog);
    QComboBox* comboBox = d->createComboBox(d->currentObject, ui.inputWidgetContainer);

    QHBoxLayout* layout = new QHBoxLayout(ui.inputWidgetContainer);
    layout->setMargin(0);
    layout->addWidget(comboBox);

    if(newPropDialog.exec() == QDialog::Rejected)
        return;

    QTreeWidgetItem* item = new QTreeWidgetItem(d->ui.propertyEditor);
    item->setText(0, comboBox->currentText());
    item->setSizeHint(0, QSize(20, 20));
    item->setSizeHint(1, QSize(20, 20));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
    d->propertyMap[d->currentObject][item->text(0)] = "";
}

void GCF::ComponentPropertyEditorPage::on_cmdRemoveProperty_clicked()
{
    QTreeWidgetItem* item = d->ui.propertyEditor->currentItem();
    if(!item || item->text(0) == "title")
        return;

    d->propertyMap[d->currentObject].remove(item->text(0));
    delete item;
}

void GCF::ComponentPropertyEditorPage::on_propertyEditor_itemChanged(QTreeWidgetItem* item, int)
{
    if(d->currentObject.isEmpty() || !item)
        return;

    d->propertyMap[d->currentObject][item->text(0)] = item->text(1);
}

void GCF::ComponentPropertyEditorPageData::loadComponentTree(GCF::AbstractComponent* comp)
{
    QTreeWidgetItem* compItem = new QTreeWidgetItem(ui.componentTree);
    GCF::ComponentGui gui = comp->componentGui();

    compItem->setText(0, comp->componentName());
    compItem->setData(0, Qt::UserRole, qVariantFromValue<GCF::ComponentGui>(gui));
    compItem->setIcon(0, qApp->style()->standardIcon(QStyle::SP_DirIcon));

    // Store the title property
    for(int i=0; i<gui.nodeCount(); i++)
    {
        GCF::ComponentGuiNode node = gui.node(i);
        if(node.type() == GCF::ComponentGuiNode::Object || node.type() == GCF::ComponentGuiNode::Widget)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(compItem);
            item->setText(0, node.name());
            item->setData(0, Qt::UserRole, node.completeName());
            item->setIcon(0, qApp->style()->standardIcon(QStyle::SP_ComputerIcon));

            propertyMap[node.completeName()]["title"] = node["title"].toString();
        }
        else if(node.type() == GCF::ComponentGuiNode::Property)
        {
            QString completePropName = node["key"].toString();
            QString objName = completePropName.section("::", 0, 0);
            QString propName = completePropName.section("::", 1, 1);
            QString propValue = node["value"].toString();
            propertyMap[objName][propName] = propValue;
        }
    }

    compItem->setExpanded(true);
}

void GCF::ComponentPropertyEditorPageData::saveProperties()
{
    GCF::AbstractComponent* firstComp = GCF::ComponentFactory::instance().components().first();
    QMap< QString, QMap<QString, QString> >::iterator it = propertyMap.begin();
    QMap< QString, QMap<QString, QString> >::iterator end = propertyMap.end();

    guiXmlMap.clear();
    while(it != end)
    {
        QString objectName = it.key();
        QMap<QString, QString> props = it.value();
        GCF::ComponentGuiNode node = firstComp->componentGui().findNode(objectName, true);
        saveProperties(node, props);
        ++it;
    }

    QMap<QString, QDomDocument>::iterator git = guiXmlMap.begin();
    QMap<QString, QDomDocument>::iterator gend = guiXmlMap.end();
    while(git != gend)
    {
        QFile file(git.key());
        file.open(QFile::WriteOnly);
        QString fc = git.value().toString(2);
        file.write(fc.toAscii());
        ++git;
    }
    guiXmlMap.clear();
}

void GCF::ComponentPropertyEditorPageData::saveProperties(GCF::ComponentGuiNode node, const QMap<QString, QString> & pm)
{
    if(!node.isValid())
        return;

    QMap<QString, QString> propMap = pm;
    QString guiFileName = QString("%1/%2").arg(GCF::AbstractComponent::guiXmlFileDirectory()).arg(node.component()->guiXmlFileName());
    QDomDocument doc;

    if(guiXmlMap.contains(guiFileName))
        doc = guiXmlMap[guiFileName];
    else
    {
        QFile guiFile(guiFileName);
        guiFile.open(QFile::ReadOnly);
        QString guiFileContents(guiFile.readAll());
        doc.setContent(guiFileContents);
        guiXmlMap[guiFileName] = doc;
    }

    // Update title and other properties that were already declared
    QDomNodeList nodes = doc.documentElement().childNodes();
    QList<QDomNode> nodesToRemove;
    for(int i=0; i<nodes.count(); i++)
    {
        QDomElement nodeE = nodes.item(i).toElement();
        if(nodeE.isNull())
            continue;

        if(nodeE.tagName()=="object" || nodeE.tagName()=="widget")
        {
            if(nodeE.attribute("name") == node.name())
            {
                nodeE.setAttribute("title", propMap["title"]);
                propMap.remove("title");
            }
        }
        else if(nodeE.tagName()=="property")
        {
            QString key = nodeE.attribute("key");
            if(key.section("::", 0, 0) != node.completeName())
                continue;

            QString key2 = key.section("::", 1, 1);
            if(propMap.contains(key2))
            {
                nodeE.setAttribute("value", propMap[key2]);
                propMap.remove(key2);
            }
            else
            {
                nodesToRemove.push_back(nodeE);
            }
        }
    }

    // Remove unwanted nodes.
    for(int i=0; i<nodesToRemove.count(); i++)
        doc.documentElement().removeChild(nodesToRemove[i]);

    // Append new properties.
    QMap<QString, QString>::iterator it = propMap.begin();
    QMap<QString, QString>::iterator end = propMap.end();
    while(it != end)
    {
        QDomElement propE = doc.createElement("property");
        propE.setAttribute("key", QString("%1::%2").arg(node.completeName()).arg(it.key()));
        propE.setAttribute("value", it.value());
        doc.documentElement().appendChild(propE);
        ++it;
    }
}

void GCF::ComponentPropertyEditorPageData::showProperties(const QString& completeName)
{
    ui.propertyEditor->clear();

    QMap<QString, QString> map = propertyMap[completeName];
    QMap<QString, QString>::iterator it = map.begin();
    QMap<QString, QString>::iterator end = map.end();
    while(it != end)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui.propertyEditor);
        item->setText(0, it.key());
        item->setText(1, it.value());
        item->setSizeHint(0, QSize(20, 20));
        item->setSizeHint(1, QSize(20, 20));
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
        ++it;
    }
}

QObject* GCF::ComponentPropertyEditorPageData::getObject(const QString& completeName)
{
    QObject* ret = 0;
    if(completeName.isEmpty())
        return ret;

    GCF::ComponentGuiNode node;
    if(!currentGui.component())
    {
        GCF::AbstractComponent* firstComp = GCF::ComponentFactory::instance().components().first();
        if(!firstComp)
            return ret;
        GCF::ComponentGui gui = firstComp->componentGui();
        node = gui.findNode(completeName, true);
    }
    else
        node = currentGui.findNode(completeName, true);
    if(!node.isValid())
        return ret;

    switch(node.type())
    {
    case GCF::ComponentGuiNode::Object:
        ret = node["QObject"].value<QObject*>();
        break;
    case GCF::ComponentGuiNode::Widget:
        ret = node["QWidget"].value<QWidget*>();
        break;
    default:
        break;
    }

    return ret;
}

QComboBox* GCF::ComponentPropertyEditorPageData::createComboBox(const QString& completeName, QWidget* parent)
{
    QComboBox* combo = new QComboBox(parent);
    QObject* object = getObject(completeName);
    combo->addItem("title");

    if(!object)
        return combo;

    QStringList propNames;
    const QMetaObject* mo = object->metaObject();
    for(int i=0; i<mo->propertyCount(); i++)
    {
        QMetaProperty prop = mo->property(i);
        propNames.push_back(prop.name());
    }

    qSort(propNames);
    combo->addItems(propNames);

    return combo;
}


