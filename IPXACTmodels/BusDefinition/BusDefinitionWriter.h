//-----------------------------------------------------------------------------
// File: BusDefinitionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2015
//
// Description:
// Writer for IP-XACT BusDefinition element.
//-----------------------------------------------------------------------------

#ifndef BUSDEFINITIONWRITER_H
#define BUSDEFINITIONWRITER_H

#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/DocumentWriter.h>

#include <IPXACTmodels/ipxactmodels_global.h>

class BusDefinition;
class VLNV;

//-----------------------------------------------------------------------------
//! Writer for IP-XACT BusDefinition element.
//-----------------------------------------------------------------------------
namespace BusDefinitionWriter
{

    /*!
     *  Writes the given bus definition into xml.
     *
     *    @param [in] writer          The xml writer to use.
     *    @param [in] busDefinition   The bus definition to write.
     */
    IPXACTMODELS_EXPORT void writeBusDefinition(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition);

    namespace Details
    {
        /*!
         *  Writes the broadcast property of a given bus definition into xml.
         *
         *    @param [in] writer          The xml writer to use.
         *    @param [in] busDefinition   The bus definition whose broadcast to write.
         */
        void writeBroadcast(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition);

        /*!
         *  Writes the bus definition extended in a given bus definition into xml.
         *
         *    @param [in] writer          The xml writer to use.
         *    @param [in] busDefinition   The bus definition whose extends property to write.
         */
        void writeExtends(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition);

        /*!
         *  Writes the maximum number of initiators allowed in a given bus definition into xml.
         *
         *    @param [in] writer          The xml writer to use.
         *    @param [in] busDefinition   The bus definition whose initiator count to write.
         */
        void writeMaximumInitiators(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition);

        /*!
         *  Writes the maximum number of targets allowed in a given bus definition into xml.
         *
         *    @param [in] writer          The xml writer to use.
         *    @param [in] busDefinition   The bus definition whose targets count to write.
         */
        void writeMaximumTargets(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition);

        /*!
         *  Writes the system group names defined in a given bus definition into xml.
         *
         *    @param [in] writer          The xml writer to use.
         *    @param [in] busDefinition   The bus definition whose system group names to write.
         */
        void writeSystemGroupNames(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition);

        /*!
         *  Writes the choices of a bus definition into xml.
         *
         *    @param [in] writer          The xml writer to use.
         *    @param [in] busDefinition   The bus definition whose choices to write.
         */
        void writeChoices(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition);

        /*!
         *  Writes the description of a bus definition into xml.
         *
         *    @param [in] writer          The xml writer to use.
         *    @param [in] busDefinition   The bus definition whose choices to write.
         */
        void writeDescription(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition);

        /*!
         *  Converts boolean value to string.
         *
         *    @param [in] value   The boolean value to convert.
         *
         *    @return     The string representation of the boolean value.
         */
        QString bool2Str(bool value);
    }
};    

#endif // BUSDEFINITIONWRITER_H