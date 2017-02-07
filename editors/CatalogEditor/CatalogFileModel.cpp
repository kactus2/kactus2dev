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

#include <common/utils.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Catalog/IpxactFile.h>

#include <QFileInfo>
#include <QIcon>
#include <QSize>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Private namespace.
//-----------------------------------------------------------------------------
namespace
{
    const QString CATEGORIES[CatalogFileColumns::CATEGORY_COUNT] =
    {
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

    const QString ICONS[CatalogFileColumns::CATEGORY_COUNT] =
    {
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
} 

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::refresh()
//-----------------------------------------------------------------------------
void CatalogFileModel::refresh(QSharedPointer<Catalog> catalog)
{
    catalog_ = catalog;
    
    beginResetModel();
    topLevelRows_.clear();
    topLevelRows_.append(catalog->getCatalogs());
    topLevelRows_.append(catalog->getBusDefinitions());
    topLevelRows_.append(catalog->getAbstractionDefinitions());
    topLevelRows_.append(catalog->getComponents());
    topLevelRows_.append(catalog->getAbstractors());
    topLevelRows_.append(catalog->getDesigns());
    topLevelRows_.append(catalog->getDesignConfigurations());
    topLevelRows_.append(catalog->getGeneratorChains());
    topLevelRows_.append(QSharedPointer<QList<QSharedPointer<IpxactFile> > >(
        new QList<QSharedPointer<IpxactFile> > ())); //<! Other, unknown items.
    
    endResetModel();

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
    for (int topRow = 0; topRow < topLevelRows_.count(); topRow++)
    {
        for (int childRow = 0; childRow < topLevelRows_.at(topRow)->count(); childRow++)
        {
            if (topLevelRows_.at(topRow)->at(childRow).data() == child.internalPointer())
            {
                return createIndex(topRow, 0, topLevelRows_.at(topRow).data());
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
                if (!isValidPath(file->getName()))
                {
                    return QColor(Qt::red);
                }
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

    addFile(droppedFile);

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

    VLNV::IPXactType type = VLNV::INVALID;

    if (index.isValid())
    {
        if (index.parent().isValid())
        {
            type = CatalogFileColumns::CATEGORY_TYPES[index.parent().row()];
        }
        else
        {
            type = CatalogFileColumns::CATEGORY_TYPES[index.row()];
        }
    }

    VLNV vlnv;    
    vlnv.setType(type);

    QSharedPointer<IpxactFile> file(new IpxactFile());
    file->setVlnv(vlnv);
    addFile(file);
   
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

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::onOpenItem()
//-----------------------------------------------------------------------------
void CatalogFileModel::onOpenItem(QModelIndex const& index)
{
    if (!index.isValid() || !index.parent().isValid())
    {
        return;
    }

    QSharedPointer<IpxactFile> file = topLevelRows_.at(index.parent().row())->at(index.row());
    VLNV fileVLNV = file->getVlnv();

    VLNV::IPXactType type = CatalogFileColumns::CATEGORY_TYPES[index.parent().row()];
    if (type == VLNV::CATALOG)
    {
        emit openCatalog(fileVLNV);
    }
    else if (type == VLNV::BUSDEFINITION)
    {
        emit openBus(fileVLNV, VLNV());
    }
    else if (type == VLNV::ABSTRACTIONDEFINITION)
    {
        QSharedPointer<AbstractionDefinition const> abstraction = 
            library_->getModelReadOnly(fileVLNV).dynamicCast<AbstractionDefinition const>();
        if (abstraction)
        {
            emit openBus(abstraction->getBusType(), fileVLNV);
        }
    }
    else if (type == VLNV::COMPONENT)
    {
        emit openComponent(fileVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::addFile()
//-----------------------------------------------------------------------------
void CatalogFileModel::addFile(QSharedPointer<IpxactFile> fileToAdd)
{
    VLNV vlnv = fileToAdd->getVlnv();

    int category = 0;
    for (category = 0; category <= CatalogFileColumns::UNKNOWN; category++)
    {
        if (vlnv.getType() == CatalogFileColumns::CATEGORY_TYPES[category])
        {
            break;
        }
    }

    QModelIndex parentIndex = index(category, 0, QModelIndex());
    int lastRow = rowCount(parentIndex);

    beginInsertRows(parentIndex, lastRow, lastRow);
    topLevelRows_.at(category)->append(fileToAdd);
    endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileModel::isValidPath()
//-----------------------------------------------------------------------------
bool CatalogFileModel::isValidPath(QString const& path) const
{
    if (path.isEmpty())
    {
        return false;
    }

    bool isValidUri = Utils::URL_VALIDITY_REG_EXP.match(path).hasMatch();
    if (isValidUri)
    {
        return true;
    }

    QFileInfo fileInfo(path);
    if (fileInfo.isRelative())
    {
        QString basePath = library_->getPath(catalog_->getVlnv());
        QString absFilePath = General::getAbsolutePath(basePath, path);

        fileInfo.setFile(absFilePath);
    }

    return fileInfo.exists();
}
