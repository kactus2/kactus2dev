//-----------------------------------------------------------------------------
// File: ModeReader.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.08.2023
//
// Description:
// Reader class for IP-XACT mode element.
//-----------------------------------------------------------------------------

#ifndef ModeREADER_H
#define ModeREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Range.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

class Mode;
class RemapPort;

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT mode element.
//-----------------------------------------------------------------------------
namespace ModeReader
{

    /*!
     *  Creates a new mode from a given view node.
     *
     *      @param [in] modeNode  XML description of the mode.
     *
     *      @return The created mode.
     */
    IPXACTMODELS_EXPORT QSharedPointer<Mode> createModeFrom(QDomNode const& modeNode);

    namespace Details
    {

        /*!
         *   Parses the mode condition.
         *
         *      @param [in] modeNode   XML description of the nodet.
         *      @param [in] newMode    The selected mode item.
         *
         */
        void parseCondition(QDomNode const& modeNode, QSharedPointer<Mode> newMode);

        /*!
         *  Read the remap ports.
         *
         *      @param [in] ModeNode  XML description of the mode.
         *      @param [in] newMode   The selected mode item.
         */
        void parsePortSlices(QDomNode const& modeNode, QSharedPointer<Mode> newMode);
       
        /*!
         *  Read the field slices.
         *
         *      @param [in] modeNode  XML description of the mode.
         *      @param [in] newMode   The selected mode item.
         */
        void parseFieldSlices(QDomNode const& modeNode, QSharedPointer<Mode> newMode);


        /*!
         *   Parses range.
         *
         *      @param [in] element    XML description of the IP-XACT element.
         *      @param [in] range      The range to read to.
         *
         */
        void parseRange(QDomElement const& element, QSharedPointer<Range> range);
    }
};

#endif // VIEWREADER_H
