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

#include "librarytreefilter.h"
#include "libraryitem.h"

#include "libraryinterface.h"

#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/busdefinition.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>

#include <QRegExp>

//-----------------------------------------------------------------------------
// Function: LibraryTreeFilter::LibraryTreeFilter()
//-----------------------------------------------------------------------------
LibraryTreeFilter::LibraryTreeFilter(LibraryInterface* handler, VLNVDialer* dialer, QObject *parent):
LibraryFilter(dialer, parent), 
    handler_(handler)
{
    Q_ASSERT(handler_);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeFilter::~LibraryTreeFilter()
//-----------------------------------------------------------------------------
LibraryTreeFilter::~LibraryTreeFilter()
{

}

//-----------------------------------------------------------------------------
// Function: LibraryTreeFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool LibraryTreeFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex itemIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    // root item is always ok
    if (!itemIndex.isValid())
        return true;

    LibraryItem* item = static_cast<LibraryItem*>(itemIndex.internalPointer());

    // get all vlnvs for the item
    QList<VLNV> list;
    item->getVLNVs(list);

    // if none of the vlnvs match the search rules
    if (!checkVLNVs(list))
    {
        return false;
    }

    foreach (VLNV vlnv, list)
    {
        if (!handler_->contains(vlnv))
        {
            continue;
        }

        QSharedPointer<LibraryComponent const> libComb = handler_->getModelReadOnly(vlnv);

        // check the type
        switch (handler_->getDocumentType(vlnv))
        {
        case VLNV::COMPONENT:
            {
                // if components are not be displayed
                if (!type().components_)
                {
                    continue;
                }
                break;
            }
        case VLNV::ABSTRACTIONDEFINITION:
            {
                // if buses are not to be displayed
                if (!type().buses_) 
                {
                    continue;
                }

                // abstraction definitions are always for hw

                // if hw is not shown then continue to check next 
                if (!implementation().hw_)
                    continue;

                else
                {
                    return true;
                }
            }
        case VLNV::BUSDEFINITION:
            {

                // if buses are not to be displayed
                if (!type().buses_)
                {
                    continue;
                }
                QSharedPointer<BusDefinition const> busDef = libComb.staticCast<BusDefinition const>();

                // if this was not supposed to show then check next one
                if  (!implementation().hw_)
                    continue;
                else
                    return true;
            }

        case VLNV::COMDEFINITION:
        case VLNV::APIDEFINITION: 
            {

                // if buses are not to be displayed
                if (!type().buses_)
                {
                    continue;
                }

                return true;
            }

        case VLNV::DESIGN:
            {
                QSharedPointer<Design> design = handler_->getDesign(vlnv);
                if (type().components_ && implementation().sw_ && 
                    design->getDesignImplementation() == KactusAttribute::SW)
                {
                    return true;
                }
            }
            // if type is one of the advanced
        default:

            // if other types should be displayed
            if (type().advanced_)
            {
                return true;
            }

            // if other types are not to be displayed then check other possible VLNVs.
            continue;
        }

        // the vlnv is for component for sure
        QSharedPointer<Component const> component = libComb.staticCast<Component const>();

        // if component does not match the filters
        if (!checkImplementation(component) || !checkHierarchy(component) || !checkFirmness(component))
        {
            continue;
        }

        // if all tests passed
        return true;
    }

    return false;
}
