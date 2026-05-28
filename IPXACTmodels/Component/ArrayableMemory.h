//-----------------------------------------------------------------------------
// File: ArrayableMemory.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 19.05.2025
//
// Description:
// Base class for IP-XACT memory elements containing the memory array element (ipxact:array).
//-----------------------------------------------------------------------------

#ifndef ARRAYABLEMEMORY_H
#define ARRAYABLEMEMORY_H

#include <IPXACTmodels/Component/MemoryArray.h>

class IPXACTMODELS_EXPORT ArrayableMemory
{
public:

    /*!
     *	The constructor
     */
    ArrayableMemory() = default;

    //! Copy constructor
    ArrayableMemory(ArrayableMemory const& other);

    //! Assignment
    ArrayableMemory& operator=(ArrayableMemory const& other);

    virtual ~ArrayableMemory() = default;
    
    /*!
     *  Get the dimension of the memory item.
     *
     *    @return The first dimension defined for the memory item.
     */
    QString getDimension() const;

    /*!
     *  Set the first dimension of the memory item.
     *
     *    @param [in] newDimension    The new value for the first dim-element.
     */
    void setDimension(QString const& newDimension);


    /*!
     *	Get the stride value. Implemented in derived classes because of different stride defaults when empty.
     *  
     *    @return  The stride value
     */
    virtual QString getStride();

    void setStride(QString const& newStride);

    QString getIndexVar();

    void setIndexVar(QString const& newIndexVar);

    /*!
     *	Get the memory array of the memory item.
     *
     * 	    @return Pointer to the memory array of the memory item.
     */
    QSharedPointer<MemoryArray> getMemoryArray() const;

    /*!
     *	Set the memory array of the memory item.
     *
     *    @param [in] newMemArray     Pointer to the memory array to set.
     */
    void setMemoryArray(QSharedPointer<MemoryArray> newMemArray);

protected:

    //! The memory array.
    QSharedPointer<MemoryArray> memoryArray_;

};

#endif // ARRAYABLEMEMORY_H
