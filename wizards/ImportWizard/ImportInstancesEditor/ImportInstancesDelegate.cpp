//-----------------------------------------------------------------------------
// File: ImportInstancesDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.04.2019
//
// Description:
// Delegate that provides editing for imported instances.
//-----------------------------------------------------------------------------

#include "ImportInstancesDelegate.h"

#include <wizards/ImportWizard/ImportInstancesEditor/ImportInstancesColumns.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QLineEdit>
#include <QFileInfo>

namespace
{
    QString const NEWCOMPONENTTEXT("<New component>");
};

//-----------------------------------------------------------------------------
// Function: ImportInstancesDelegate::ImportInstancesDelegate()
//-----------------------------------------------------------------------------
ImportInstancesDelegate::ImportInstancesDelegate(QSharedPointer<const Component> workingComponent,
    QString const& defaultPath, LibraryInterface* libraryAccess, QObject* parent):
QStyledItemDelegate(parent),
instanceComponentMatches_(),
instanceData_(),
workingComponent_(workingComponent),
defaultPath_(defaultPath),
libraryAccess_(libraryAccess)
{

}

//-----------------------------------------------------------------------------
// Function: ImportInstancesDelegate::setupVLNVEditor()
//-----------------------------------------------------------------------------
void ImportInstancesDelegate::setupVLNVEditors(
    QMultiMap<QString, QSharedPointer<const Component>> instanceComponentMatches,
    QVector<InstanceData::instanceData> const& newInstances)
{
    instanceComponentMatches_ = instanceComponentMatches;
    instanceData_ = newInstances;
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ImportInstancesDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == ImportInstancesColumns::VLNV)
    {
        return createVLNVSelector(index, parent);
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesDelegate::createVLNVSelector()
//-----------------------------------------------------------------------------
QComboBox* ImportInstancesDelegate::createVLNVSelector(QModelIndex const& selectionIndex, QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->addItem(NEWCOMPONENTTEXT);

    QModelIndex const& instanceNameIndex =
        selectionIndex.sibling(selectionIndex.row(), ImportInstancesColumns::INSTANCE);
    QString instanceName = instanceNameIndex.data(Qt::DisplayRole).toString();

    QList<QSharedPointer<Component const> > componentMatches = instanceComponentMatches_.values(instanceName);
    for (auto component : componentMatches)
    {
        combo->addItem(component->getVlnv().toString());
    }

    return combo;
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ImportInstancesDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const 
{
    if (index.column() == ImportInstancesColumns::VLNV)
    {
        QString itemVLNV = index.data(Qt::DisplayRole).toString();
        QComboBox* vlnvCombo = qobject_cast<QComboBox*>(editor);

        int comboIndex = vlnvCombo->findText(itemVLNV);
        if (comboIndex < 0 && !itemVLNV.isEmpty())
        {
            vlnvCombo->addItem(itemVLNV);
            vlnvCombo->setCurrentIndex(vlnvCombo->findText(itemVLNV));
        }
        else
        {
            vlnvCombo->setCurrentIndex(comboIndex);
        }
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesDelegate::setModelData()
//-----------------------------------------------------------------------------
void ImportInstancesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    QModelIndex const& index) const 
{
    if (index.column() == ImportInstancesColumns::VLNV)
    {
        QComboBox* vlnvCombo = qobject_cast<QComboBox*>(editor);
        QString newVLNVText = vlnvCombo->currentText();
        QString oldVLNVText = index.data(Qt::DisplayRole).toString();

        if (newVLNVText.compare(NEWCOMPONENTTEXT) == 0)
        {
            QModelIndex instanceNameIndex = index.sibling(index.row(), ImportInstancesColumns::INSTANCE);
            QString instanceName = instanceNameIndex.data(Qt::DisplayRole).toString();

            VLNV newVLNV = workingComponent_->getVlnv();
            newVLNV.setName(instanceName);
            newVLNV.setVersion(QStringLiteral("1.0"));

            newVLNVText = newVLNV.toString();
        }

        if (newVLNVText.compare(oldVLNVText) != 0)
        {
            model->setData(index, newVLNVText, Qt::DisplayRole);

            setNewXMLPath(index, newVLNVText, model);
        }
    }
    else if (index.column() == ImportInstancesColumns::PATH || index.column() == ImportInstancesColumns::XML_PATH)
    {
        QLineEdit* pathEditor = qobject_cast<QLineEdit*>(editor);
        if (pathEditor)
        {
            QString path = pathEditor->text();

            if (index.column() == ImportInstancesColumns::PATH && !path.isEmpty())
            {
                QFileInfo fileInfo(path);
                if (!fileInfo.exists() || !fileInfo.isFile())
                {
                    return;
                }
                else
                {
                    emit checkForNewLibraryLocation(path);
                }
            }
            else if (index.column() == ImportInstancesColumns::XML_PATH)
            {
                if (path.isEmpty())
                {
                    return;
                }
                else
                {
                    emit checkForNewLibraryLocation(path);
                }
            }

            emit changeIndexItemTooltip(index, path);
        }
    }
    
    QStyledItemDelegate::setModelData(editor, model, index);
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesDelegate::setNewXMLPath()
//-----------------------------------------------------------------------------
void ImportInstancesDelegate::setNewXMLPath(QModelIndex const& vlnvIndex, QString const& newVLNVText,
    QAbstractItemModel* model) const
{
    QModelIndex xmlPathIndex = vlnvIndex.sibling(vlnvIndex.row(), ImportInstancesColumns::XML_PATH);
    QModelIndex const& instanceNameIndex = vlnvIndex.sibling(vlnvIndex.row(), ImportInstancesColumns::INSTANCE);
    QString instanceName = instanceNameIndex.data(Qt::DisplayRole).toString();

    QString newXMLPath("");
    bool itemIsEditable = false;

    for (auto component : instanceComponentMatches_.values(instanceName))
    {
        if (newVLNVText.compare(component->getVlnv().toString()) == 0)
        {
            newXMLPath = libraryAccess_->getPath(component->getVlnv());
            break;
        }
    }

    if (newXMLPath.isEmpty())
    {
        VLNV newItemVLNV = workingComponent_->getVlnv();
        newItemVLNV.setName(getModuleNameForInstance(instanceName));
        newItemVLNV.setVersion(QStringLiteral("1.0"));

        QString newItemFileName =
            newItemVLNV.getName() + QLatin1Char('.') + newItemVLNV.getVersion() + QLatin1String(".xml");
        newXMLPath =
            defaultPath_ + newItemVLNV.toString(QStringLiteral("/")) + QLatin1Char('/') + newItemFileName;

        itemIsEditable = true;
    }

    model->setData(xmlPathIndex, newXMLPath);
    emit changeItemEditableStatus(xmlPathIndex, itemIsEditable);
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesDelegate::getModuleNameForInstance()
//-----------------------------------------------------------------------------
QString ImportInstancesDelegate::getModuleNameForInstance(QString const& instanceName) const
{
    for (auto const& instance : instanceData_)
    {
        if (instance.instanceName_.compare(instanceName) == 0)
        {
            return instance.moduleName_;
        }
    }

    return QString("");
}
