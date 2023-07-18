//-----------------------------------------------------------------------------
// File: NameGroupReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Reader for name group element.
//-----------------------------------------------------------------------------

#ifndef NAMEGROUPREADER_H
#define NAMEGROUPREADER_H

#include "NameGroup.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Reader for name group element.
//-----------------------------------------------------------------------------
namespace NameGroupReader
{

    /*!
     *  Reads the name group.
     *
     *      @param [in] nameGroupNode   XML description of the name group.
     *      @param [in] nameGroup       The new name group.
     */
    void parseNameGroup(QDomNode const& nameGroupNode, QSharedPointer<NameGroup> nameGroup);

    namespace Details
    {
        /*!
         *  Reads and creates a name for name group.
         *
         *      @param [in] nameGroupNode   XML description of the name group.
         *
         *      @return The name of the name group.
         */
        QString parseName(QDomNode const& nameGroupNode);

        /*!
         *  Reads and creates a display name for name group.
         *
         *      @param [in] nameGroupNode   XML description of the name group.
         *
         *      @return The display name of the name group.
         */
        QString parseDisplayName(QDomNode const& nameGroupNode);

        /*!
         *  Reads and creates a short description for name group.
         *
         *      @param [in] nameGroupNode   XML description of the name group.
         *
         *      @return The short description of the name group.
         */
        QString parseShortDescription(QDomNode const& nameGroupNode);

        /*!
         *  Reads and creates a description for name group.
         *
         *      @param [in] nameGroupNode   XML description of the name group.
         *
         *      @return The description of the name group.
         */
        QString parseDescription(QDomNode const& nameGroupNode);
    }
};

#endif // NAMEGROUPREADER_H