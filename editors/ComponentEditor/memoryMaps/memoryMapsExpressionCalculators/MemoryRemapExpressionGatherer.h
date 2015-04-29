//-----------------------------------------------------------------------------
// File: MemoryRemapExpressionsGatherer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from a memory remap and its address blocks.
//-----------------------------------------------------------------------------

#ifndef MEMORYREMAPEXPRESSIONGATHERER_H
#define MEMORYREMAPEXPRESSIONGATHERER_H

#include <IPXACTmodels/AbstractMemoryMap.h>

#include <QSharedPointer>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Gathers expressions from a memory remap and its address blocks.
//-----------------------------------------------------------------------------
class MemoryRemapExpressionGatherer
{

public:

    /*!
     *  The constructor.
     */
    MemoryRemapExpressionGatherer();

    /*!
     *  The destructor.
     */
    virtual ~MemoryRemapExpressionGatherer();

    /*!
     *  Get the expressions from a given memory remap.
     *
     *      @param [in] memoryRemap     The given memory remap.
     *
     *      @return A list containing all the expressions used in a memory remap
     */
    QStringList getExpressions(QSharedPointer<AbstractMemoryMap> memoryRemap) const;

private:
	
	//! No copying
    MemoryRemapExpressionGatherer(const MemoryRemapExpressionGatherer& other);

	//! No assignment
    MemoryRemapExpressionGatherer& operator=(const MemoryRemapExpressionGatherer& other);

};

#endif // MEMORYREMAPEXPRESSIONGATHERER_H
