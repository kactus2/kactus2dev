//-----------------------------------------------------------------------------
// File: NameGroupReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Reader class for name group element.
//-----------------------------------------------------------------------------

#ifndef NAMEGROUPREADER_H
#define NAMEGROUPREADER_H

#include "NameGroup.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Reader class for name group element.
//-----------------------------------------------------------------------------
class NameGroupReader
{


public:

    /*!
     *  The constructor.
     *
     */
    NameGroupReader();

    /*!
     *  The destructor.
     */
    ~NameGroupReader();

    /*!
     *  Reads the name group.
     *
     *      @param [in] nameGroupNode   XML description of the name group.
     *      @param [in] nameGroup       The new name group.
     */
    static void parseNameGroup(QDomNode const& nameGroupNode, QSharedPointer<NameGroup> nameGroup);

    /*!
     *  Reads and creates a name for name group.
     *
     *      @param [in] nameGroupNode   XML description of the name group.
     *
     *      @return The name of the name group.
     */
    static QString parseName(QDomNode const& nameGroupNode);

    /*!
     *  Reads and creates a display name for name group.
     *
     *      @param [in] nameGroupNode   XML description of the name group.
     *
     *      @return The display name of the name group.
     */
    static QString parseDisplayName(QDomNode const& nameGroupNode);
  
    /*!
     *  Reads and creates a short description for name group.
     *
     *      @param [in] nameGroupNode   XML description of the name group.
     *
     *      @return The short description of the name group.
     */
    static QString parseShortDescription(QDomNode const& nameGroupNode);

    /*!
     *  Reads and creates a description for name group.
     *
     *      @param [in] nameGroupNode   XML description of the name group.
     *
     *      @return The description of the name group.
     */
    static QString parseDescription(QDomNode const& nameGroupNode);

private:

    // Disable copying.
    NameGroupReader(NameGroupReader const& rhs);
    NameGroupReader& operator=(NameGroupReader const& rhs);
};

#endif // NAMEGROUPREADER_H