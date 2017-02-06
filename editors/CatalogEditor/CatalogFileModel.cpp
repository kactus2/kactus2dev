//-----------------------------------------------------------------------------
// File: CatalogFileModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.02.2017
//
// Description:
// Model class for ipxactFiles in a catalog.
//-----------------------------------------------------------------------------

#include "CatalogFileModel.h"

#include "CatalogFileColumns.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Catalog/IpxactFile.h>

#include <QIcon>
#include <QSize>
#include <QMimeData>

namespace
{
    const QString CATEGORIES[CatalogFileColumns::CATEGORY_COUNT] = {
        QLatin1String("Catalogs"),
        QLatin1String("Bus definitions"),
        QLatin1String("Abstraction definitions"),
        QLatin1String("Components"),
        QLatin1String("Abstractors"),
        QLatin1String("Designs"),
        QLatin1String("Design configurations"),
        QLatin1String("Generator chains"),
        QLatin1String("Unknown")
    };

    const QString ICONS[CatalogFileColumns::CATEGORY_COUNT] = {
        QLatin1String(":icons/common/graphics/catalog.png"),
        QLatin1String(":icons/common/graphics/new-bus.png"),
        QLatin1String(":icons/common/graphics/abs-def.png"),        
        QLatin1String(":icons/common/graphics/hw-component.png"),
        QLatin1String(":icons/common/graphics/connect.png"),
        QLatin1String(":icons/common/graphics/hw-design.png"),
        QLatin1String(":icons/common/graphics/configuration.png"),
        QLatin1String(":icons/common/graphics/generatorChain.png"),
        QLatin1String(":icons/common/graphics/exclamation--frame.png")
    };
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::CatalogFileModel()
//-----------------------------------------------------------------------------
CatalogFileModel::CatalogFileModel(LibraryInterface* library, QObject *parent) :
QAbstractItemModel(parent),
    catalog_(),
    library_(library),
    topLevelTypes_(),
    topLevelRows_(),
    locked_(true)
{

}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::~CatalogFileModel()
//-----------------------------------------------------------------------------
CatalogFileModel::~CatalogFileModel()
{

}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::setProtection()
//-----------------------------------------------------------------------------
void CatalogFileModel::setProtection(bool locked)
{
    locked_ = locked;

    QVector<int> roles(Qt::ForegroundRole);

    for (int i = 0; i < CatalogFileColumns::CATEGORY_COUNT; i++)
    {
        QModelIndex parentIndex = index(i, 0, QModelIndex());
        emit dataChanged(index(0, 0, parentIndex), index(rowCount(parentIndex), CatalogFileColumns::PATH, parentIndex), roles);
    }    
} 

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::refresh()
//-----------------------------------------------------------------------------
void CatalogFileModel::refresh(QSharedPointer<Catalog> catalog)
{
    catalog_ = catalog;
    
    beginResetModel();
    topLevelTypes_.clear();
    topLevelRows_.clear();

    topLevelTypes_.append(VLNV::CATALOG);
    topLevelRows_.append(catalog->getCatalogs());

    topLevelTypes_.append(VLNV::BUSDEFINITION);
    topLevelRows_.append(catalog->getBusDefinitions());

    topLevelTypes_.append(VLNV::ABSTRACTIONDEFINITION);
    topLevelRows_.append(catalog->getAbstractionDefinitions());

    topLevelTypes_.append(VLNV::COMPONENT);
    topLevelRows_.append(catalog->getComponents());

    topLevelTypes_.append(VLNV::ABSTRACTOR);
    topLevelRows_.append(catalog->getAbstractors());

    topLevelTypes_.append(VLNV::DESIGN);
    topLevelRows_.append(catalog->getDesigns());

    topLevelTypes_.append(VLNV::DESIGNCONFIGURATION);
    topLevelRows_.append(catalog->getDesignConfigurations());

    topLevelTypes_.append(VLNV::GENERATORCHAIN);
    topLevelRows_.append(catalog->getGeneratorChains());

    topLevelTypes_.append(VLNV::INVALID);
    topLevelRows_.append(QSharedPointer<QList<QSharedPointer<IpxactFile> > >(new QList<QSharedPointer<IpxactFile> > ()));
    
    endResetModel();

    Q_ASSERT(topLevelTypes_.count() == CatalogFileColumns::CATEGORY_COUNT);
    Q_ASSERT(topLevelRows_.count() == CatalogFileColumns::CATEGORY_COUNT);
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::columnCount()
//-----------------------------------------------------------------------------
int CatalogFileModel::columnCount(QModelIndex const& /*parent*/) const
{
    return CatalogFileColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::rowCount()
//-----------------------------------------------------------------------------
int CatalogFileModel::rowCount(QModelIndex const& parent) const
{
    if (!parent.isValid())
    {
        return topLevelRows_.count();
    }

    for (int i = 0; i < topLevelRows_.count(); i++)
    {
        if (parent.internalPointer() == topLevelRows_.at(i).data())
        {
            return topLevelRows_.at(i)->count();
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::index()
//-----------------------------------------------------------------------------
QModelIndex CatalogFileModel::index(int row, int column, QModelIndex const& parent) const
{
    if (!parent.isValid() && 0 <= row && row < topLevelRows_.count())
    {
        return createIndex(row, column, topLevelRows_.at(row).data());
    }

    for (int i = 0; i < topLevelRows_.count(); i++)
    {
        if (parent.internalPointer() == topLevelRows_.at(i).data() && row < topLevelRows_.at(i)->count())
        {
            return createIndex(row, column, topLevelRows_.at(i)->at(row).data());
        }
    }

    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::parent()
//-----------------------------------------------------------------------------
QModelIndex CatalogFileModel::parent(QModelIndex const& child) const
{
    for (int i = 0; i < topLevelRows_.count(); i++)
    {
        for (int j = 0; j < topLevelRows_.at(i)->count(); j++)
        {
            if (topLevelRows_.at(i)->at(j).data() == child.internalPointer())
            {
                return createIndex(i, 0, topLevelRows_.at(i).data());
            }
        }
    }

    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::headerData()
//-----------------------------------------------------------------------------
QVariant CatalogFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            if (section == CatalogFileColumns::VENDOR)
            {
                return tr("Vendor");
            }
            else if (section == CatalogFileColumns::LIBRARY)
            {
                return tr("Library");
            }
            else if (section == CatalogFileColumns::NAME)
            {
                return tr("Name");
            }
            else if (section == CatalogFileColumns::VERSION)
            {
                return tr("Version");
            }
            else if (section == CatalogFileColumns::PATH)
            {
                return tr("Path");
            }          
        }

        else if (role == Qt::TextAlignmentRole)
        {
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::data()
//-----------------------------------------------------------------------------
QVariant CatalogFileModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (!index.parent().isValid())
    {
        if (index.column() == 0 && role == Qt::DisplayRole)
        {
            return CATEGORIES[index.row()];
        }
        else if (role == Qt::BackgroundRole)
        {
            return QColor(QStringLiteral("gainsboro"));
        }
        else if (role == Qt::DecorationRole && index.column() == CatalogFileColumns::TREE_NAVIGATION)
        {
            return QIcon(ICONS[index.row()]);
        }
    }

    else
    {
        QSharedPointer<IpxactFile> file = topLevelRows_.at(index.parent().row())->at(index.row());

        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            if (index.column() == CatalogFileColumns::VENDOR)
            {
                return file->getVlnv().getVendor();
            }
            if (index.column() == CatalogFileColumns::LIBRARY)
            {
                return file->getVlnv().getLibrary();
            }
            if (index.column() == CatalogFileColumns::NAME)
            {
                return file->getVlnv().getName();
            }
            if (index.column() == CatalogFileColumns::VERSION)
            {
                return file->getVlnv().getVersion();
            }
            if (index.column() == CatalogFileColumns::PATH)
            {
                return file->getName();
            }
        }

        if (role == Qt::ForegroundRole && index.column() != CatalogFileColumns::TREE_NAVIGATION)
        {
            if (locked_)
            {
                return QColor(Qt::lightGray);
            }
            if (index.column() == CatalogFileColumns::PATH)
            {
                //TODO: check path exists.
            }
            else if (!library_->contains(file->getVlnv()))
            {
                return QColor(Qt::red);
            }
        }
    }

    // Unsupported role.
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::setData()
//-----------------------------------------------------------------------------
bool CatalogFileModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (locked_ || !index.isValid() || !index.parent().isValid())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        QSharedPointer<IpxactFile> file = topLevelRows_.at(index.parent().row())->at(index.row());
        VLNV fileVLNV = file->getVlnv();

        if (index.column() == CatalogFileColumns::VENDOR)
        {
            fileVLNV.setVendor(value.toString());
            file->setVlnv(fileVLNV);
        }
        else if (index.column() == CatalogFileColumns::LIBRARY)
        {
            fileVLNV.setLibrary(value.toString());
            file->setVlnv(fileVLNV);
        }
        else if (index.column() == CatalogFileColumns::NAME)
        {
            fileVLNV.setName(value.toString());
            file->setVlnv(fileVLNV);
        }
        else if (index.column() == CatalogFileColumns::VERSION)
        {
            fileVLNV.setVersion(value.toString());
            file->setVlnv(fileVLNV);
        }
        else if (index.column() == CatalogFileColumns::PATH)
        {
            file->setName(value.toString());
        }

        emit dataChanged(index, index);
        emit contentChanged();

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags CatalogFileModel::flags(QModelIndex const& index) const
{
    Qt::ItemFlags itemFlags = Qt::ItemIsEnabled;

    if (!locked_)
    {
        itemFlags |= Qt::ItemIsDropEnabled;
    }

    if (!locked_ && index.parent().isValid() && index.column() != CatalogFileColumns::TREE_NAVIGATION)
    {
        itemFlags |= Qt::ItemIsEditable; 
    }

    if (!index.parent().isValid() || index.column() != CatalogFileColumns::TREE_NAVIGATION)
    {
        itemFlags |= Qt::ItemIsSelectable;
    }

    return itemFlags;
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList CatalogFileModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions CatalogFileModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::dropMimeData()
//-----------------------------------------------------------------------------
bool CatalogFileModel::dropMimeData(QMimeData const* data, Qt::DropAction action, int /*row*/, int /*column*/,
    QModelIndex const& /*parent*/)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    QVariant variant = data->imageData();
    if (!variant.canConvert<VLNV>())
    {
        return false;
    }

    VLNV vlnv = variant.value<VLNV>();
    QString path = General::getRelativePath(library_->getPath(catalog_->getVlnv()), library_->getPath(vlnv));

    QSharedPointer<IpxactFile> droppedFile(new IpxactFile);
    droppedFile->setVlnv(vlnv);
    droppedFile->setName(path);

    int category = topLevelTypes_.indexOf(vlnv.getType());
    if (category == -1)
    {
        category = CatalogFileColumns::UNKNOWN;
    }

    QModelIndex parentIndex = index(category, 0, QModelIndex());
    int lastRow = rowCount(parentIndex);

    beginInsertRows(parentIndex, lastRow, lastRow);
    topLevelRows_.at(category)->append(droppedFile);
    endInsertRows();

    emit contentChanged();

    return true;
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::onAddItem()
//-----------------------------------------------------------------------------
void CatalogFileModel::onAddItem(QModelIndex const& index)
{
    if (locked_)
    {
        return;
    }

    QModelIndex parentIndex;

    if (index.isValid())
    {
        if (index.parent().isValid())
        {
            parentIndex = index.parent();
        }
        else
        {
            parentIndex = index;
        }
    }

    int category = CatalogFileColumns::UNKNOWN;
    if (parentIndex.isValid())
    {
        category = parentIndex.row();
    }
    else
    {
        parentIndex = CatalogFileModel::index(category, 0, QModelIndex());
    }

    int lastRow = rowCount(parentIndex);

    beginInsertRows(parentIndex, lastRow, lastRow);
    topLevelRows_.at(category)->append(QSharedPointer<IpxactFile>(new IpxactFile()));
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::onRemoveItem()
//-----------------------------------------------------------------------------
void CatalogFileModel::onRemoveItem(QModelIndex const& index)
{
    if (locked_ || !index.isValid() || !index.parent().isValid())
    {
        return;
    }

    int category = index.parent().row();
    int row = index.row();

    beginRemoveRows(index.parent(), row, row);
    topLevelRows_.at(category)->removeAt(row);
    endRemoveRows();

    emit contentChanged();
}
