//-----------------------------------------------------------------------------
// File: ModeWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 07.08.2023
//
// Description:
// Writer class for IP-XACT mode element.
//-----------------------------------------------------------------------------

#ifndef ModeWRITER_H
#define ModeWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

class Mode;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT mode element.
//-----------------------------------------------------------------------------
namespace ModeWriter
{

    /*!
     *  Write a mode to an XML file.
     *
     *    @param [in] writer  The used xml writer.
     *    @param [in] view    The mode to be written.
     */
    IPXACTMODELS_EXPORT void writeMode(QXmlStreamWriter& writer, QSharedPointer<Mode> mode);

    namespace Details
    {
        /*!
         *  Write the mode condition to an XML file.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] view    The mode whose condition to write.
         */
        void writeCondition(QXmlStreamWriter& writer, QSharedPointer<Mode> mode);

        /*!
         *  Write the mode port slices to an XML file.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] view    The mode whose port slices to write.
         */
        void writePortSlices(QXmlStreamWriter& writer, QSharedPointer<Mode> mode);
       
        /*!
         *  Write the mode field slices to an XML file.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] view    The mode whose field slices to write.
         */
        void writeFieldSlices(QXmlStreamWriter& writer, QSharedPointer<Mode> mode);
    }
};

#endif // ModeWRITER_H