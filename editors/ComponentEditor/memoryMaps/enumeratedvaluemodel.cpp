//-----------------------------------------------------------------------------
// File: enumeratedvaluemodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 28.08.2012
//
// Description:
// The model to manage the enumerated values of a field.
//-----------------------------------------------------------------------------

#include "enumeratedvaluemodel.h"
#include "EnumeratedValueColumns.h"

#include <IPXACTmodels/Component/EnumeratedValue.h>

#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>

#include <QColor>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::EnumeratedValueModel()
//-----------------------------------------------------------------------------
EnumeratedValueModel::EnumeratedValueModel(
    QSharedPointer<QList<QSharedPointer<EnumeratedValue> > > enumeratedValues,
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator, QObject *parent):
QAbstractTableModel(parent),
    enumValues_(enumeratedValues),
    enumeratedValueValidator_(enumeratedValueValidator)
{

}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::~EnumeratedValueModel()
//-----------------------------------------------------------------------------
EnumeratedValueModel::~EnumeratedValueModel()
{

}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::rowCount()
//-----------------------------------------------------------------------------
int EnumeratedValueModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return enumValues_->size();
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::columnCount()
//-----------------------------------------------------------------------------
int EnumeratedValueModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const
{
	if (parent.isValid())
    {
		return 0;
	}
    return EnumeratedValueColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags EnumeratedValueModel::flags( const QModelIndex& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::headerData()
//-----------------------------------------------------------------------------
QVariant EnumeratedValueModel::headerData( int section, Qt::Orientation orientation,
    int role /*= Qt::DisplayRole*/ ) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}
	if (Qt::DisplayRole == role)
    {
        if (section == EnumeratedValueColumns::NAME_COLUMN)
        {
            return tr("Enumeration name");
        }
        else if (section == EnumeratedValueColumns::DISPLAY_NAME)
        {
            return tr("Display name");
        }
        else if (section == EnumeratedValueColumns::VALUE_COLUMN)
        {
            return tr("Value");
        }
        else if (section == EnumeratedValueColumns::USAGE_COLUMN)
        {
            return tr("Usage");
        }
        else if (section == EnumeratedValueColumns::DESCRIPTION)
        {
            return tr("Description");
        }
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::data()
//-----------------------------------------------------------------------------
QVariant EnumeratedValueModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const
{
	if (!index.isValid())
    {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= enumValues_->size())
    {
		return QVariant();
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
	}

	else if (Qt::ForegroundRole == role)
    {
        if (enumeratedValueValidator_->validate(enumValues_->at(index.row())))
        {
            return QColor("black");
        }
        else
        {
            return QColor("red");
        }
	}

	else if (Qt::BackgroundRole == role)
    {
        if (index.column() == EnumeratedValueColumns::NAME_COLUMN ||
            index.column() == EnumeratedValueColumns::VALUE_COLUMN)
        {
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
        }
	}
	else
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::setData()
//-----------------------------------------------------------------------------
bool EnumeratedValueModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ )
{
	if (!index.isValid())
    {
		return false;
	}
	else if (index.row() < 0 || index.row() >= enumValues_->size())
    {
		return false;
	}

	if (Qt::EditRole == role)
    {
        if (index.column() == EnumeratedValueColumns::NAME_COLUMN)
        {
            enumValues_->at(index.row())->setName(value.toString());
        }
        else if (index.column() == EnumeratedValueColumns::DISPLAY_NAME)
        {
            enumValues_->at(index.row())->setDisplayName(value.toString());
        }
        else if (index.column() == EnumeratedValueColumns::VALUE_COLUMN)
        {
            enumValues_->at(index.row())->setValue(value.toString());
        }
        else if (index.column() == EnumeratedValueColumns::USAGE_COLUMN)
        {
            enumValues_->at(index.row())->setUsage
                (EnumeratedValue::str2Usage(value.toString(), EnumeratedValue::UNKNOWNUSAGE));
        }
        else if (index.column() == EnumeratedValueColumns::DESCRIPTION)
        {
            enumValues_->at(index.row())->setDescription(value.toString());
        }
        else
        {
            return false;
        }

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::onAddItem()
//-----------------------------------------------------------------------------
void EnumeratedValueModel::onAddItem( const QModelIndex& index )
{
	int row = enumValues_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	enumValues_->insert(row, QSharedPointer<EnumeratedValue>(new EnumeratedValue()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit enumAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::onRemoveItem()
//-----------------------------------------------------------------------------
void EnumeratedValueModel::onRemoveItem( const QModelIndex& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid())
    {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= enumValues_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	enumValues_->removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit enumRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant EnumeratedValueModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == EnumeratedValueColumns::NAME_COLUMN)
    {
        return enumValues_->at(index.row())->name();
    }
    else if (index.column() == EnumeratedValueColumns::DISPLAY_NAME)
    {
        return enumValues_->at(index.row())->displayName();
    }
    else if (index.column() == EnumeratedValueColumns::VALUE_COLUMN)
    {
        return enumValues_->at(index.row())->getValue();
    }
    else if (index.column() == EnumeratedValueColumns::USAGE_COLUMN)
    {
        return EnumeratedValue::usage2Str(enumValues_->at(index.row())->getUsage());
    }
    else if (index.column() == EnumeratedValueColumns::DESCRIPTION)
    {
        return enumValues_->at(index.row())->description();
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::onCopyRows()
//-----------------------------------------------------------------------------
void EnumeratedValueModel::onCopyRows(QModelIndexList indexList)
{
    QList<QSharedPointer<EnumeratedValue> > copiedEnumeratedValues;
    foreach (QModelIndex index, indexList)
    {
        QSharedPointer<EnumeratedValue> enumValue = enumValues_->at(index.row());
        copiedEnumeratedValues.append(enumValue);
    }

    QVariant enumValueVariant;
    enumValueVariant.setValue(copiedEnumeratedValues);
    
    QMimeData* newMimeData = new QMimeData();
    
    newMimeData->setData("text/xml/ipxact:enumeratedValue", QByteArray());
    newMimeData->setImageData(enumValueVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::onPasteRows()
//-----------------------------------------------------------------------------
void EnumeratedValueModel::onPasteRows()
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<EnumeratedValue> > >())
        {
            QList<QSharedPointer<EnumeratedValue> > newEnumeratedValeus =
                pasteVariant.value<QList<QSharedPointer<EnumeratedValue> > >();

            int rowBegin = enumValues_->size();
            int rowEnd = rowBegin + newEnumeratedValeus.size() - 1;

            beginInsertRows(QModelIndex(), rowBegin, rowEnd);

            foreach(QSharedPointer<EnumeratedValue> copiedEnumValue, newEnumeratedValeus)
            {
                QSharedPointer<EnumeratedValue> newEnumValue (new EnumeratedValue(*copiedEnumValue.data()));
                newEnumValue->setName(getUniqueName(newEnumValue->name()));

                enumValues_->append(newEnumValue);

                emit enumAdded(enumValues_->size() - 1);
            }

            endInsertRows();

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::getUniqueName()
//-----------------------------------------------------------------------------
QString EnumeratedValueModel::getUniqueName(QString const& originalName)
{
    QString name = originalName;
    int trailingNumber = 1;

    bool match =  true;
    while (match)
    {
        match = false;        
        for(int row = 0; row < enumValues_->size(); row++)
        {
            if (name.compare(enumValues_->at(row)->name()) == 0)
            {
                match = true;
                name = originalName + "_" + QString::number(trailingNumber);
                trailingNumber++;
            }
        }
    }

    return name;
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluemodel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList EnumeratedValueModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:enumeratedValue";

    return types;
}
