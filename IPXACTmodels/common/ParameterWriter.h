//-----------------------------------------------------------------------------
// File: ParameterWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 31.07.2015
//
// Description:
// Writer for IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#ifndef PARAMETERWRITER_H
#define PARAMETERWRITER_H

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Document.h>

class Parameter;
class VendorExtension;

//-----------------------------------------------------------------------------
//! Writer for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
namespace ParameterWriter
{

    /*!
     *  Writes the given parameter into xml.
     *
     *    @param [in] writer      The xml writer to use.
     *    @param [in] parameter   The parameter to write.
     *    @param [in] docRevision The document std revision.
     */
    IPXACTMODELS_EXPORT void writeParameter(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter, 
        Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Writes the attributes of a given parameter into xml.
         *
         *    @param [in] writer      The xml writer to use.
         *    @param [in] parameter   The parameter whose attributes to write.
         */
        void writeAttributes(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter);

        /*!
         *  Writes the name group of a given parameter into xml.
         *
         *    @param [in] writer      The xml writer to use.
         *    @param [in] parameter   The parameter whose name group to write.
         *    @param [in] docRevision The document std revision.
         */
        void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter,
            Document::Revision docRevision);

        /*!
         *  Writes the vectors of a given parameter into xml.
         *
         *    @param [in] writer      The xml writer to use.
         *    @param [in] parameter   The parameter whose vectors to write.
         *    @param [in] docRevision The document std revision.
         */
        void writeVectors(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter, 
            Document::Revision docRevision);

        /*!
         *  Writes the arrays of a given parameter into xml.
         *
         *    @param [in] writer      The xml writer to use.
         *    @param [in] parameter   The parameter whose arrays to write.
         *    @param [in] docRevision The document std revision.
         */
        void writeArrays(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter,
            Document::Revision docRevision);

        /*!
         *  Writes the value of a given parameter into xml.
         *
         *    @param [in] writer      The xml writer to use.
         *    @param [in] parameter   The parameter whose value to write.
         */
        void writeValue(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter);
    }
};

#endif // PARAMETERWRITER_H
