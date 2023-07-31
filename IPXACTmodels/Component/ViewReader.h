//-----------------------------------------------------------------------------
// File: ViewReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Reader for IP-XACT view element.
//-----------------------------------------------------------------------------

#ifndef VIEWREADER_H
#define VIEWREADER_H

#include "View.h"

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Document.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader for IP-XACT view element.
//-----------------------------------------------------------------------------
namespace ViewReader
{

    /*!
     *  Creates a new view from a given view node.
     *
     *      @param [in] viewNode    XML description of the view.
     *      @param [in] docRevision The IP-XACT standard revision to comply to.
     *
     *      @return The created view.
     */
    IPXACTMODELS_EXPORT QSharedPointer<View> createViewFrom(QDomNode const& viewNode, Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Reads the presence.
         *
         *      @param [in] viewNode    XML description of the view.
         *      @param [in] newView     The new view item.
         */
        void parseIsPresent(QDomNode const& viewNode, QSharedPointer<View> newView);

        /*!
         *  Reads the envIdentifiers.
         *
         *      @param [in] viewNode    XML description of the view.
         *      @param [in] newView     The new view item.
         */
        void parseEnvIdentifiers(QDomNode const& viewNode, QSharedPointer<View> newView);

        /*!
         *  Reads the instantiation references.
         *
         *      @param [in] viewNode    XML description of the view.
         *      @param [in] newView     The new view item.
         */
        void parseInstantiationRefs(QDomNode const& viewNode, QSharedPointer<View> newView);
    }
};

#endif // VIEWREADER_H