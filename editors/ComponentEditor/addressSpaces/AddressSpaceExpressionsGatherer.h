//-----------------------------------------------------------------------------
// File: AddressSpaceExpressionsGatherer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from an address space and its memory maps.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEEXPRESSIONGATHERER_H
#define ADDRESSSPACEEXPRESSIONGATHERER_H

#include <IPXACTmodels/addressspace.h>

#include <QSharedPointer>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Gathers expressions from an address space and its memory maps.
//-----------------------------------------------------------------------------
class AddressSpaceExpressionGatherer
{

public:

    /*!
     *  The constructor.
     */
    AddressSpaceExpressionGatherer();

    /*!
     *  The destructor.
     */
    virtual ~AddressSpaceExpressionGatherer();

    /*!
     *  Get the expressions from a given address space.
     *
     *      @param [in] addressSpace    The given address space.
     *
     *      @return A list containing all the expressions used in an address space.
     */
    QStringList getExpressions(QSharedPointer<AddressSpace> addressSpace) const;

private:
	
	//! No copying
    AddressSpaceExpressionGatherer(const AddressSpaceExpressionGatherer& other);

	//! No assignment
    AddressSpaceExpressionGatherer& operator=(const AddressSpaceExpressionGatherer& other);
};

#endif // ADDRESSSPACEEXPRESSIONGATHERER_H
