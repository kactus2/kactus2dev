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

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT Choice element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ChoiceReader
{

public:

    /*!
     *  Creates a new Choice from a given Choice node.
     *
     *      @param [in] choiceNode    XML description of the choice.
     */
    QSharedPointer<Choice> createChoiceFrom(QDomNode const& choiceNode) const;

};

#endif // ChoiceReader_H
