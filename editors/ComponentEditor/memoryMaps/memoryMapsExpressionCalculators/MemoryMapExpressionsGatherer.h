//-----------------------------------------------------------------------------
// File: MemoryMapExpressionsGatherer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from a memory map and its memory remaps.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPEXPRESSIONGATHERER_H
#define MEMORYMAPEXPRESSIONGATHERER_H

#include <IPXACTmodels/memorymap.h>

#include <QSharedPointer>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Gathers expressions from a memory map and its memory remaps.
//-----------------------------------------------------------------------------
class MemoryMapExpressionGatherer
{

public:

    /*!
     *  The constructor.
     */
    MemoryMapExpressionGatherer();

    /*!
     *  The destructor.
     */
    virtual ~MemoryMapExpressionGatherer();

    /*!
     *  Get the expressions from a given memory map.
     *
     *      @param [in] memoryMap   The given memory map.
     *
     *      @return A list containing all the expressions used in a memory map.
     */
    QStringList getExpressions(QSharedPointer<MemoryMap> memoryMap) const;

private:
	
	//! No copying
    MemoryMapExpressionGatherer(const MemoryMapExpressionGatherer& other);

	//! No assignment
    MemoryMapExpressionGatherer& operator=(const MemoryMapExpressionGatherer& other);

};

#endif // MEMORYMAPEXPRESSIONGATHERER_H
