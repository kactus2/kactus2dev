//-----------------------------------------------------------------------------
// File: AddressBlockExpressionGatherer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from an address block and its registers.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKEXPRESSIONSGATHERER_H
#define ADDRESSBLOCKEXPRESSIONSGATHERER_H

#include <IPXACTmodels/addressblock.h>

#include <QSharedPointer>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Gathers expressions from an address block and its registers.
//-----------------------------------------------------------------------------
class AddressBlockExpressionGatherer
{

public:

    /*!
     *  The constructor.
     */
    AddressBlockExpressionGatherer();

    /*!
     *  The destructor.
     */
    virtual ~AddressBlockExpressionGatherer();

    /*!
     *  Get the expressions from the given address block.
     *
     *      @param [in] currentAddressBlock     The given address block.
     *
     *      @return A list of all the expressions used in address block.
     */
    QStringList getExpressions(QSharedPointer<AddressBlock> currentAddressBlock) const;

private:
	
	//! No copying
    AddressBlockExpressionGatherer(const AddressBlockExpressionGatherer& other);

	//! No assignment
    AddressBlockExpressionGatherer& operator=(const AddressBlockExpressionGatherer& other);

};

#endif // ADDRESSBLOCKEXPRESSIONSGATHERER_H
