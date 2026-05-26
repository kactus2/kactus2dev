//-----------------------------------------------------------------------------
// File: HierarchyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 07.04.2026
//
// Description:
// Extended hierarchy model containing GUI functionality for Kactus2
//-----------------------------------------------------------------------------

#include "HierarchyModel.h"

#include <KactusAPI/include/KactusColors.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <common/KactusUtils.h>

#include <QStringBuilder>

HierarchyModel::HierarchyModel(LibraryInterface* handler, QObject* parent) :
    HierarchyModelBase(handler, parent)
{

}

QVariant HierarchyModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid())
    {
    	return QVariant();
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

    if (role == Qt::DisplayRole) 
    {
        if (index.column() == HierarchyModelBase::OBJECT_COLUMN)
        {
            VLNV const& vlnv = item->getVLNV();
            if (vlnv.isValid())
            {
                return vlnv.toString();
            }
        }
        else if (index.column() == HierarchyModelBase::INSTANCE_COLUMN)
        {
            int count = item->instanceCount();
            if (count > 0)
            {
                return count;
            }
        }        
        
        return QVariant();
    }
    else if (role == Qt::ForegroundRole)
    {
    	if (item->isValid()) 
        {
    		return KactusColors::REGULAR_TEXT;
        }
    	else
        {
    		return KactusColors::ERROR_COLOR;
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        VLNV vlnv = item->getVLNV();

        QString text = "<b>Vendor:</b> " % vlnv.getVendor() % "<br>" %
                       "<b>Library:</b> " % vlnv.getLibrary() % "<br>" %
                       "<b>Name:</b> " % vlnv.getName() % "<br>" %
                       "<b>Version:</b> " % vlnv.getVersion() % "<br>";

        QSharedPointer<Document const> document = handler_->getModelReadOnly(vlnv);
        
        text += "<br><b>Compatibility:</b> " + Document::toString(document->getRevision());

        if (document != 0 && !document->getDescription().isEmpty())
        {
            text += "<br><b>Description:</b><br>" % document->getDescription();
        }

        text += QString("<br><b>File Path:</b><br>%1").arg(handler_->getPath(vlnv));
        return text;
    }
    else if (role == Qt::DecorationRole && index.column() == HierarchyModelBase::OBJECT_COLUMN)
    {
        if (item->type() == HierarchyItem::COMPONENT)
        {
            if (item->getImplementation() == KactusAttribute::SYSTEM)
            {
                return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/system-component.png"));
            }
            else if (item->getImplementation() == KactusAttribute::SW)
            {
                if (item->isHierarchical())
                {
                    return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/hier-sw-component.png"));
                }
                else
                {
                    return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/sw-component48x48.png"));
                }
            }
            else
            {
                if (item->isHierarchical()) 
                {
                    return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/hier-hw-component.png"));
                }
                else
                {
                    return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/hw-component.png"));
                }		
            }
        }
        else if (item->type() == HierarchyItem::CATALOG)
        {
            return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/catalog.png"));
        }
        else if (item->type() == HierarchyItem::COMDEFINITION)
        {
            return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/new-com_definition.png"));
        }
        else if (item->type() == HierarchyItem::APIDEFINITION)
        {
            return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/new-api_definition.png"));
        }
    	else if (item->type() == HierarchyItem::HW_DESIGN)
        {
    		return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/hw-design.png"));
    	}
    	else if (item->type() == HierarchyItem::SW_DESIGN || item->type() == HierarchyItem::SYS_DESIGN)
        {
            return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/sw-design48x48.png"));
    	}
        else if (item->type() == HierarchyItem::ABSDEFINITION)
        {
            return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/abs-def.png"));
        }
        else if (item->type() == HierarchyItem::BUSDEFINITION)
        {
            return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/bus-def.png"));
        }
        else
        {
            return KactusUtils::getIconStyledToTheme(QStringLiteral(":/icons/common/graphics/cross.png"));
        }
    }

    return QVariant();
}
