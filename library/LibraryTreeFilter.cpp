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
LibraryFilter(handler, parent)
{

}

//-----------------------------------------------------------------------------
// Function: LibraryTreeFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool LibraryTreeFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    auto itemIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    // root item is always ok.
    if (!itemIndex.isValid())
    {
        return true;
    }

    auto item = static_cast<LibraryItem*>(itemIndex.internalPointer());

    // get all vlnvs for the item
    QVector<VLNV> list = item->getVLNVs();

    // if none of the vlnvs match the filter rules
    if (!checkVLNVs(list))
    {
        return false;
    }

    for (VLNV const& vlnv : list)
    {
        if (checkType(vlnv) && filterRegularExpression().match(vlnv.toString()).hasMatch() &&
            checkValidity(item->isValid()))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeFilter::checkType()
//-----------------------------------------------------------------------------
bool LibraryTreeFilter::checkType(VLNV const& vlnv) const
{
    auto libraryAccess = getLibraryInterface();

    VLNV::IPXactType documentType = libraryAccess->getDocumentType(vlnv);
    QSharedPointer<Document const> document = libraryAccess->getModelReadOnly(vlnv);
    
    if (checkTags(document) == false)
    {
        return false;
    }

    if (documentType == VLNV::COMPONENT && type().components_)
    {
        QSharedPointer<Component const> component = document.staticCast<Component const>();

        if (checkImplementation(component) && checkHierarchy(component) &&
            checkFirmness(component))
        {
            return true;
        }
    }

    else if (documentType == VLNV::CATALOG && type().catalogs_)
    {
        return true;
    }

    else if ((documentType == VLNV::BUSDEFINITION || documentType == VLNV::ABSTRACTIONDEFINITION) &&
        type().buses_)
    {
        if (implementation().hw_)
        {
            return true;
        }
    }

    else if ((documentType == VLNV::COMDEFINITION || documentType == VLNV::APIDEFINITION) &&
        type().apis_)
    {
        return true;
    }

    else if (documentType == VLNV::DESIGN)
    {
        QSharedPointer<Design> design = libraryAccess->getDesign(vlnv);

        if (type().advanced_ || (type().components_ && implementation().sw_ &&
            design->getImplementation() == KactusAttribute::SW))
        {
            return true;
        }
    }

    else if (type().advanced_)
    {
        return true;
    }

    return false;
}
