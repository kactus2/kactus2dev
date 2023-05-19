//-----------------------------------------------------------------------------
// File: ChoiceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 19.05.2023
//
// Description:
// Writer for choice element.
//-----------------------------------------------------------------------------

#ifndef CHOICEWRITER_H
#define CHOICEWRITER_H

#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/common/Enumeration.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

namespace ChoiceWriter
{

    /*
     *  Writes a single choice to XML. 
     *  
     *      @param [in] writer    The XML writer.
     *      @param [in] choice    The choice to be written.
     */
    void writeChoice(QXmlStreamWriter& writer, QSharedPointer<Choice> choice);

    namespace Details
    {
        /*
         *  Writes a single choice enumeration to XML.
         *
         *      @param [in] writer    The XML writer.
         *      @param [in] choice    The enumeration to be written.
         */
        void writeEnumeration(QXmlStreamWriter& writer, QSharedPointer<Enumeration> enumeration);
    }
}

#endif // CHOICEWRITER_H