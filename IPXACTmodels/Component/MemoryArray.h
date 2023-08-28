//-----------------------------------------------------------------------------
// File: MemoryArray.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.7.2023
//
// Description:
// Describes the ipxact:array element found in array block, register, and field.
//-----------------------------------------------------------------------------

#ifndef MEMORYARRAY_H
#define MEMORYARRAY_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Describes the ipxact:array element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryArray
{

public:
    // ipxact:dim
    struct Dimension
    {
        QString value_;
        QString indexVar_;
    };

    /*!
     *	The constructor.     
     */
    MemoryArray();

    MemoryArray(MemoryArray const& other);

    MemoryArray& operator=(MemoryArray const& other);

    /*!
     *	The destructor.
     */
    virtual ~MemoryArray() = default;

    /*!
     *	Get the array dimensions.
     *	    
     * 	    @return The dimensions of the array.
     */
    QSharedPointer<QList<QSharedPointer<Dimension> > > getDimensions() const;

    /*!
     *	Set the dimensions of the array.
     *  
     *      @param [in] dimensions     The dimensions to set.
     *	    
     */
    void setDimensions(QSharedPointer<QList<QSharedPointer<Dimension> > > dimensions);

    /*!
     *	Get the array stride.
     *  
     * 	    @return The array stride, or bitStride if the array is within a field element.
     */
    QString getStride() const;

    /*!
     *	Set the array stride or bitStride, if in a field element.
     *  
     *      @param [in] stride     Description
     */
    void setStride(QString const& stride);

private:

    //! List of dimensions in the array.
    QSharedPointer<QList<QSharedPointer<Dimension> > > dimensions_ = 
        QSharedPointer<QList<QSharedPointer<Dimension> > >(new QList<QSharedPointer<Dimension> >);

    //! ipxact:stride or ipxact:bitStride.
    QString stride_;
};

#endif // MEMORYARRAY_H
