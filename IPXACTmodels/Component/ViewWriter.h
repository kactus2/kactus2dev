//-----------------------------------------------------------------------------
// File: ViewWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Writer for IP-XACT view element.
//-----------------------------------------------------------------------------

#ifndef VIEWWRITER_H
#define VIEWWRITER_H

#include "View.h"

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer for IP-XACT view element.
//-----------------------------------------------------------------------------
namespace ViewWriter
{

    /*!
     *  Write a view to an XML file.
     *
     *      @param [in] writer       The used xml writer.
     *      @param [in] view         The view to be written.
     *      @param [in] docRevision  The IP-XACT standard revision to comply to.
     */
    IPXACTMODELS_EXPORT void writeView(QXmlStreamWriter& writer, QSharedPointer<View> view, 
        Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Writes the presence to an XML file.
         *
         *      @param [in] writer  The used xml writer.
         *      @param [in] view    The view to be written.
         */
        void writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<View> view);

        /*!
         *  Writes the envIdentifiers to an XML file.
         *
         *      @param [in] writer  The used xml writer.
         *      @param [in] view    The view to be written.
         */
        void writeEnvIdentifiers(QXmlStreamWriter& writer, QSharedPointer<View> view);

        /*!
         *  Writes the instantiation references to an XML file.
         *
         *      @param [in] writer  The used xml writer.
         *      @param [in] view    The view to be written.
         */
        void writeInstantiationReferences(QXmlStreamWriter& writer, QSharedPointer<View> view);
    }
};

#endif // VIEWWRITER_H