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

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/kactusExtensions/SWView.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>

#include <QFileInfo>

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
            QSharedPointer<Document> newModel = library_->getModel(oldVLNV);
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
        setIcon(object, item);

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
// Function: HierarchicalSaveBuildStrategy::setIcon()
//-----------------------------------------------------------------------------
void HierarchicalSaveBuildStrategy::setIcon(QObject* object, QTreeWidgetItem* item) const
{
    if (object->property("VLNVType") == "Component")
    {
        if (object->property("implementation") == "HW")
        {
            item->setIcon(HierarchicalSaveColumns::VLNV, QIcon(":/icons/common/graphics/hw-component.png"));
        }
        else
        {
            item->setIcon(HierarchicalSaveColumns::VLNV, QIcon(":/icons/common/graphics/sw-component24x24.png"));
        }
    }
    else
    {
        if (object->property("implementation") == "HW")
        {
            item->setIcon(HierarchicalSaveColumns::VLNV, QIcon(":/icons/common/graphics/hw-design.png"));
        }
        else if (object->property("implementation") == "System")
        {
            item->setIcon(HierarchicalSaveColumns::VLNV, QIcon(":/icons/common/graphics/system-design.png"));
        }
        else
        {
            item->setIcon(HierarchicalSaveColumns::VLNV, QIcon(":/icons/common/graphics/sw-design24x24.png"));
        }
    }
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
void HierarchicalSaveBuildStrategy::updateReferencesFromTo(QSharedPointer<Document> model, 
    QTreeWidgetItem* childItem) const
{
    VLNV childVLNV = VLNV(VLNV::COMPONENT, childItem->text(HierarchicalSaveColumns::VLNV));
    childVLNV.setType(library_->getDocumentType(childVLNV));

    VLNV newChildVLNV = VLNV(childVLNV.getType(), childItem->text(HierarchicalSaveColumns::SAVE_AS_VLNV));

    if (model->getVlnv().getType() == VLNV::COMPONENT)
    {
        updateComponentReferences(model.dynamicCast<Component>(), childVLNV, newChildVLNV);
    }
    else if (model->getVlnv().getType() == VLNV::DESIGN)
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
    foreach (QSharedPointer<View> view, *component->getViews())
    {
        if (view->isHierarchical())
        {
            VLNV hierarchyReference;

            QString designConfigurationName = view->getDesignConfigurationInstantiationRef();
            foreach(QSharedPointer<DesignConfigurationInstantiation> instantiation, 
                *component->getDesignConfigurationInstantiations())
            {
                if (instantiation->name() == designConfigurationName)
                {
                    hierarchyReference = *instantiation->getDesignConfigurationReference();

                    QSharedPointer<Document> configModel = library_->getModel(hierarchyReference);
                    QSharedPointer<DesignConfiguration> config = configModel.dynamicCast<DesignConfiguration>(); 

                    VLNV configVLNV(VLNV::DESIGNCONFIGURATION, updatedReference.toString());
                    configVLNV.setName(configVLNV.getName() + "cfg");

                    config->setVlnv(configVLNV);
                    config->setDesignRef(updatedReference);

                    instantiation->getDesignConfigurationReference()->setVendor(configVLNV.getVendor());
                    instantiation->getDesignConfigurationReference()->setLibrary(configVLNV.getLibrary());
                    instantiation->getDesignConfigurationReference()->setName(configVLNV.getName());
                    instantiation->getDesignConfigurationReference()->setVersion(configVLNV.getVersion());

                    saveToLibrary(hierarchyReference, config);

                    break;
                }
            }

            QString designName = view->getDesignInstantiationRef();
            foreach(QSharedPointer<DesignInstantiation> instantiation, *component->getDesignInstantiations())
            {
                if (instantiation->name() == designName)
                {
                    instantiation->getDesignReference()->setVendor(updatedReference.getVendor());
                    instantiation->getDesignReference()->setLibrary(updatedReference.getLibrary());
                    instantiation->getDesignReference()->setName(updatedReference.getName());
                    instantiation->getDesignReference()->setVersion(updatedReference.getVersion());
                    break;
                }
            }
        }
    }

    foreach (QSharedPointer<SWView> view, component->getSWViews())
    {
        QSharedPointer<Document> configModel = library_->getModel(view->getHierarchyRef());
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

    foreach (QSharedPointer<SystemView> view, component->getSystemViews())
    {
        QSharedPointer<Document> configModel = library_->getModel(view->getHierarchyRef());
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
    foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
    {
        if (*instance->getComponentRef() == childVLNV)
        {
            instance->setComponentRef(QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(newChildVLNV)));
        }
    }

    foreach (QSharedPointer<SWInstance> instance, design->getSWInstances())
    {
        if (*instance->getComponentRef() == childVLNV)
        {
            instance->setComponentRef(
                QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(newChildVLNV)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::saveToLibrary()
//-----------------------------------------------------------------------------
void HierarchicalSaveBuildStrategy::saveToLibrary(VLNV const& previousReference,
    QSharedPointer<Document> model) const
{
    if (!library_->contains(model->getVlnv()))
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
            path = savePath_ + "/" + model->getVlnv().toString("/");
        }

        library_->writeModelToFile(path, model);
    }

    emit itemSaved();
}
