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

#include <QObject>
#include <QDomNode>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Reader class for name group element.
//-----------------------------------------------------------------------------
class NameGroupReader : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent of this reader.
     */
    NameGroupReader(QObject* parent = 0);

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
    void parseNameGroup(QDomNode const& nameGroupNode, QSharedPointer<NameGroup> nameGroup) const;

    /*!
     *  Reads and creates a name for name group.
     *
     *      @param [in] nameGroupNode   XML description of the name group.
     *
     *      @return The name of the name group.
     */
    QString parseName(QDomNode const& nameGroupNode) const;

    /*!
     *  Reads and creates a display name for name group.
     *
     *      @param [in] nameGroupNode   XML description of the name group.
     *
     *      @return The display name of the name group.
     */
    QString parseDisplayName(QDomNode const& nameGroupNode) const;

    /*!
     *  Reads and creates a description for name group.
     *
     *      @param [in] nameGroupNode   XML description of the name group.
     *
     *      @return The description of the name group.
     */
    QString parseDescription(QDomNode const& nameGroupNode) const;

private:

    // Disable copying.
    NameGroupReader(NameGroupReader const& rhs);
    NameGroupReader& operator=(NameGroupReader const& rhs);
};

#endif // NAMEGROUPREADER_H