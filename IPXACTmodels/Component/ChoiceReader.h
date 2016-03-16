//-----------------------------------------------------------------------------
// File: ChoiceReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Reader class for IP-XACT Choice element.
//-----------------------------------------------------------------------------

#ifndef ChoiceReader_H
#define ChoiceReader_H

#include "Choice.h"

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT Choice element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ChoiceReader : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    ChoiceReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ChoiceReader();

    /*!
     *  Creates a new Choice from a given Choice node.
     *
     *      @param [in] ChoiceNode    XML description of the vhannel.
     */
    QSharedPointer<Choice> createChoiceFrom(QDomNode const& ChoiceNode) const;

private:

    //! No copying allowed.
    ChoiceReader(ChoiceReader const& rhs);
    ChoiceReader& operator=(ChoiceReader const& rhs);
};

#endif // ChoiceReader_H
