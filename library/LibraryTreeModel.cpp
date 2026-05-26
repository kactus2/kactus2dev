//-----------------------------------------------------------------------------
// File: LibraryTreeModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 20.3.2026
//
// Description:
// Wrapper of library model for Kactus2 GUI purposes.
//-----------------------------------------------------------------------------

#include "LibraryTreeModel.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>

#include <KactusAPI/include/KactusColors.h>
#include <KactusAPI/include/LibraryHandler.h>
#include <common/KactusUtils.h>

LibraryTreeModel::LibraryTreeModel(LibraryInterface* handler, QObject *parent)
    : LibraryModel(handler, parent)
{

}

QVariant LibraryTreeModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    if (role == Qt::ForegroundRole)
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

        // if item can identify a single library object
        if (vlnv.isValid())
        {
            QString text = "<b>Vendor:</b> " + vlnv.getVendor() + "<br>" +
                           "<b>Library:</b> " + vlnv.getLibrary() + "<br>" +
                           "<b>Name:</b> " + vlnv.getName() + "<br>" +
                           "<b>Version:</b> " + vlnv.getVersion() + "<br>";

            QSharedPointer<Document const> document = handler_->getModelReadOnly(vlnv);

            text += "<br><b>Compatibility:</b> " + Document::toString(document->getRevision());

            if (document != 0 && !document->getDescription().isEmpty())
            {
                text += "<br><b>Description:</b><br>" + document->getDescription();
            }

            text += "<br><b>File Path:</b><br>" + handler_->getPath(vlnv);
            return text;
        }

        // if item did not represent a single vlnv item
        return QVariant();
    }

    else if (role == Qt::DecorationRole)
    {
        VLNV vlnv = item->getVLNV();

        // if item represents a single vlnv object
        if (vlnv.isValid())
        {
            VLNV::IPXactType documentType = vlnv.getType();
            if (documentType == VLNV
                ::COMPONENT)
            {
                QSharedPointer<Component const> component =
                    handler_->getModelReadOnly(vlnv).staticCast<Component const>();

                if (component != 0)
                {
                    if (component->getImplementation() == KactusAttribute::SYSTEM)
                    {
                        return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/system-component.png");
                    }
                    else if (component->getImplementation() == KactusAttribute::SW)
                    {
                        if (component->isHierarchical())
                        {
                            return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/hier-sw-component.png");
                        }
                        else
                        {
                            return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/sw-component48x48.png");
                        }
                    }
                    else
                    {
                        if (component->isHierarchical())
                        {
                            return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/hier-hw-component.png");
                        }
                        else
                        {
                            return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/hw-component.png");
                        }
                    }
                }

                return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/hw-component.png");
            }

            else if (documentType == VLNV::ABSTRACTIONDEFINITION)
            {
                return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/abs-def.png");
            }

            else if (documentType == VLNV::BUSDEFINITION)
            {
                return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/bus-def.png");
            }

            else if (documentType == VLNV::CATALOG)
            {
                return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/catalog.png");
            }

            else if (documentType == VLNV::COMDEFINITION)
            {
                return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/new-com_definition.png");
            }

            else if (documentType == VLNV::APIDEFINITION)
            {
                return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/new-api_definition.png");
            }

            else if (documentType == VLNV::DESIGN)
            {
                // Determine the design type.
                QSharedPointer<const Design> design = handler_->getModelReadOnly<Design>(vlnv);
                if (design && (design->getImplementation() == KactusAttribute::SW ||
                               design->getImplementation() == KactusAttribute::SYSTEM))
                {
                    return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/sw-design48x48.png");
                }
                else
                {
                    return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/hw-design.png");
                }
            }

            else if (documentType == VLNV::DESIGNCONFIGURATION)
            {
                return KactusUtils::getIconStyledToTheme(":/icons/common/graphics/configuration.png");
            }
        }
    }

    return LibraryModel::data(index, role);
}
