//-----------------------------------------------------------------------------
// File: HierarchicalSaveBuildStrategy.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.05.2015
//
// Description:
// Builds a tree model for saving documents hierarchically.
//-----------------------------------------------------------------------------

#include "HierarchicalSaveBuildStrategy.h"

#include "HierarchicalSaveColumns.h"

#include "SaveAsItem.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/librarycomponent.h>

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::HierarchicalSaveBuildStrategy()
//-----------------------------------------------------------------------------
HierarchicalSaveBuildStrategy::HierarchicalSaveBuildStrategy(LibraryInterface* library, QObject* parent): 
QObject(parent), library_(library), saveMode_(CURRENT_DIRECTORY), savePath_()
{

}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::~HierarchicalSaveBuildStrategy()
//-----------------------------------------------------------------------------
HierarchicalSaveBuildStrategy::~HierarchicalSaveBuildStrategy()
{

}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::build()
//-----------------------------------------------------------------------------
QTreeWidgetItem* HierarchicalSaveBuildStrategy::build(QObject* rootObject) const
{
    if (rootObject && rootObject->property("VLNVType") != "DesignConfiguration")
    {
        return createItemAndItsChildren(rootObject);
    }
    else
    {
        return new QTreeWidgetItem();
    }
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::saveItem()
//-----------------------------------------------------------------------------
void HierarchicalSaveBuildStrategy::saveItem(QTreeWidgetItem* item) const
{
    if (shouldSaveItem(item))
    {
        VLNV oldVLNV = VLNV(VLNV::COMPONENT, item->text(HierarchicalSaveColumns::VLNV));
        oldVLNV.setType(library_->getDocumentType(oldVLNV));

        VLNV newVLNV = VLNV(oldVLNV.getType(), item->text(HierarchicalSaveColumns::SAVE_AS_VLNV));

        if (library_->contains(oldVLNV))
        {
            QSharedPointer<LibraryComponent> newModel = library_->getModel(oldVLNV);
            newModel->setVlnv(newVLNV);

            int childCount = item->childCount();
            for (int i = 0; i < childCount; i++)
            {
                QTreeWidgetItem* childItem = item->child(i);

                if (shouldSaveItem(childItem))
                {
                    updateReferencesFromTo(newModel, childItem);
                    saveItem(childItem);
                }
            }

            saveToLibrary(oldVLNV, newModel);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::setSaveMode()
//-----------------------------------------------------------------------------
void HierarchicalSaveBuildStrategy::setSaveMode(SaveMode mode)
{
    saveMode_ = mode;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::setSavePath()
//-----------------------------------------------------------------------------
void HierarchicalSaveBuildStrategy::setSavePath(QString const& path)
{
    savePath_ = path;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::createItemAndItsChildren()
//-----------------------------------------------------------------------------
QTreeWidgetItem* HierarchicalSaveBuildStrategy::createItemAndItsChildren(QObject* rootObject) const
{
    QTreeWidgetItem* rootItem = createItem(rootObject);

    createChildItems(rootObject, rootItem);

    return rootItem;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::createItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* HierarchicalSaveBuildStrategy::createItem(QObject* object) const
{
    QTreeWidgetItem* item = new SaveAsItem(0);

    QString vlnvString = object->objectName();
    QRegularExpression vlnvExpression("^(.+?):(.+?):(.+?):(.+?)$");

    QRegularExpressionMatch vlnvMatch = vlnvExpression.match(vlnvString);

    if (!vlnvMatch.hasMatch())
    {
        setInvalid(item);
    }
    else
    {
        item->setText(HierarchicalSaveColumns::VLNV, vlnvString);

        if (object->property("VLNVType") == "Component")
        {
            item->setIcon(HierarchicalSaveColumns::VLNV, QIcon(":/icons/common/graphics/hw-component.png"));
        }
        else
        {
            item->setIcon(HierarchicalSaveColumns::VLNV, QIcon(":/icons/common/graphics/hw-design.png"));
        }

        QString suggestedVLNV = generateUnusedVLNV(vlnvMatch.captured());
        item->setText(HierarchicalSaveColumns::SAVE_AS_VLNV, suggestedVLNV);
    }

    item->setCheckState(HierarchicalSaveColumns::SAVE_AS_VLNV, Qt::Checked);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    return item;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::setInvalid()
//-----------------------------------------------------------------------------
void HierarchicalSaveBuildStrategy::setInvalid(QTreeWidgetItem* item) const
{
    item->setText(HierarchicalSaveColumns::VLNV, QObject::tr("Item not found"));
    item->setIcon(HierarchicalSaveColumns::VLNV, QIcon(":/icons/common/graphics/exclamation.png"));
    item->setText(HierarchicalSaveColumns::SAVE_AS_VLNV, ":::");
}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::generateUnusedVLNV()
//-----------------------------------------------------------------------------
QString HierarchicalSaveBuildStrategy::generateUnusedVLNV(QString const& previousVLNV) const
{
    QString newVlnv = previousVLNV;

    QRegularExpression trailingNumber("\\d+$");
    QRegularExpressionMatch numberMatch = trailingNumber.match(previousVLNV);

    if (!numberMatch.hasMatch())
    {
        newVlnv.append("_1");
        numberMatch = trailingNumber.match(newVlnv);
    }

    while (numberMatch.hasMatch() && library_->contains(VLNV(VLNV::COMPONENT, newVlnv)))
    {
        QString nextNumber = QString::number(numberMatch.captured().toInt() + 1);
        newVlnv.replace(newVlnv.length() - numberMatch.capturedLength(), numberMatch.capturedLength(), nextNumber);

        numberMatch = trailingNumber.match(newVlnv);
    }

    return newVlnv;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::createChildItems()
//-----------------------------------------------------------------------------
void HierarchicalSaveBuildStrategy::createChildItems(QObject* object, QTreeWidgetItem* treeItem) const
{
    foreach (QObject* child, object->children())
    {
        if (child->property("VLNVType") == "DesignConfiguration")
        {
            foreach (QObject* designNode, child->children())
            {
                QTreeWidgetItem* childItem = build(designNode);
                treeItem->addChild(childItem);
            }
        }
        else if (hasNoChildWithVLNV(treeItem, child->objectName()))
        {
            QTreeWidgetItem* childItem = build(child);
            treeItem->addChild(childItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::hasNoChildWithVLNV()
//-----------------------------------------------------------------------------
bool HierarchicalSaveBuildStrategy::hasNoChildWithVLNV(QTreeWidgetItem* item, QString const& vlnv) const
{
    int childCount = item->childCount();
    for (int i = 0; i < childCount; i++)
    {
        if (item->child(i)->text(HierarchicalSaveColumns::VLNV) == vlnv)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::shouldSaveItem()
//-----------------------------------------------------------------------------
bool HierarchicalSaveBuildStrategy::shouldSaveItem(QTreeWidgetItem* item) const
{
    return item && item->checkState(HierarchicalSaveColumns::SAVE_AS_VLNV) == Qt::Checked;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalSaveBuildStrategy::updateReferencesTo()
//-----------------------------------------------------------------------------
void HierarchicalSaveBuildStrategy::updateReferencesFromTo(QSharedPointer<LibraryComponent> model, 
    QTreeWidgetItem* childItem) const
{
    VLNV childVLNV = VLNV(VLNV::COMPONENT, childItem->text(HierarchicalSaveColumns::VLNV));
    childVLNV.setType(library_->getDocumentType(childVLNV));

    VLNV newChildVLNV = VLNV(childVLNV.getType(), childItem->text(HierarchicalSaveColumns::SAVE_AS_VLNV));

    if (model->getVlnv()->getType() == VLNV::COMPONENT)
    {
        updateComponentReferences(model.dynamicCast<Component>(), childVLNV, newChildVLNV);
    }
    else if (model->getVlnv()->getType() == VLNV::DESIGN)
    {
        updateDesignReferences(model.dynamicCast<Design>(), childVLNV, newChildVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::updateComponentReferences()
//-----------------------------------------------------------------------------
void HierarchicalSaveBuildStrategy::updateComponentReferences(QSharedPointer<Component> component, 
    VLNV const& reference, VLNV const& updatedReference) const
{
    foreach (QString viewName, component->getHierViews())
    {
        View* view = component->findView(viewName);

        QSharedPointer<LibraryComponent> configModel = library_->getModel(view->getHierarchyRef());
        QSharedPointer<DesignConfiguration> config = configModel.dynamicCast<DesignConfiguration>(); 

        if (view->getHierarchyRef() == reference)
        {
            view->setHierarchyRef(updatedReference);
        }
        else if (config && config->getDesignRef() == reference)
        {
            VLNV configVLNV(VLNV::DESIGNCONFIGURATION, updatedReference.toString());
            configVLNV.setName(configVLNV.getName() + "cfg");

            config->setVlnv(configVLNV);
            config->setDesignRef(updatedReference);

            view->setHierarchyRef(configVLNV);

            saveToLibrary(view->getHierarchyRef(), config);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::updateDesignReferences()
//-----------------------------------------------------------------------------
void HierarchicalSaveBuildStrategy::updateDesignReferences(QSharedPointer<Design> design,
    VLNV childVLNV, VLNV newChildVLNV) const
{
    QList<ComponentInstance> updatedInstances;

    foreach (ComponentInstance instance, design->getComponentInstances())
    {
        if (instance.getComponentRef() == childVLNV)
        {
            instance.setComponentRef(newChildVLNV);
        }

        updatedInstances.append(instance);
    }

    design->setComponentInstances(updatedInstances);
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::saveToLibrary()
//-----------------------------------------------------------------------------
void HierarchicalSaveBuildStrategy::saveToLibrary(VLNV const& previousReference,
    QSharedPointer<LibraryComponent> model) const
{
    if (!library_->contains(*model->getVlnv()))
    {
        QString path;
        if (saveMode_ == CURRENT_DIRECTORY)
        {
            QFileInfo fileinfo(library_->getPath(previousReference));
            path = fileinfo.absolutePath();
        }
        else if (saveMode_ == SINGLE_DIRECTORY)
        {
            path = savePath_;
        }
        else //if (saveMode_ == COMMON_ROOT_DIRECTORY)
        {
            path = savePath_ + "/" + model->getVlnv()->toString("/");
        }

        library_->writeModelToFile(path, model);
    }

    emit itemSaved();
}
