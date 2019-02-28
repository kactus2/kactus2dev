//-----------------------------------------------------------------------------
// File: librarytreefilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// The class that does the filtering and sorting of items for the library tree view.
//-----------------------------------------------------------------------------

#include "LibraryTreeFilter.h"

#include "LibraryItem.h"
#include "LibraryInterface.h"

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: LibraryTreeFilter::LibraryTreeFilter()
//-----------------------------------------------------------------------------
LibraryTreeFilter::LibraryTreeFilter(LibraryInterface* handler, QObject *parent):
LibraryFilter(parent), 
    handler_(handler)
{
    Q_ASSERT(handler_);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool LibraryTreeFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex itemIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    // root item is always ok.
    if (!itemIndex.isValid())
    {
        return true;
    }

    LibraryItem* item = static_cast<LibraryItem*>(itemIndex.internalPointer());

    // get all vlnvs for the item
    QVector<VLNV> list = item->getVLNVs();

    // if none of the vlnvs match the search rules
    if (!checkVLNVs(list))
    {
        return false;
    }

    for (VLNV const& vlnv : list)
    {
        QSharedPointer<Document const> document = handler_->getModelReadOnly(vlnv);
        VLNV::IPXactType documentType = handler_->getDocumentType(vlnv);

        if (documentType == VLNV::COMPONENT)
        {
            if (type().components_)
            {
                QSharedPointer<Component const> component = document.staticCast<Component const>();

                return checkImplementation(component) && checkHierarchy(component) && checkFirmness(component);                
            }
        }

        else if (documentType == VLNV::CATALOG)
        {
            return type().catalogs_;
        }

        else if (documentType == VLNV::ABSTRACTIONDEFINITION)
        {
            return type().buses_ && implementation().hw_;
        }

        else if (documentType == VLNV::BUSDEFINITION)
        {
            return type().buses_ && implementation().hw_;
        }

        else if (documentType == VLNV::COMDEFINITION || documentType == VLNV::APIDEFINITION)
        {
            return type().apis_;
        }

        else if (documentType == VLNV::DESIGN)
        {
            QSharedPointer<Design> design = handler_->getDesign(vlnv);

            return type().advanced_ || (type().components_ && implementation().sw_ &&
                design->getImplementation() == KactusAttribute::SW);
        }

        else // if type is one of the advanced
        {
            return type().advanced_;
        }
    }

    return false;
}
