//-----------------------------------------------------------------------------
// File: MemoryBlockFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.02.2022
//
// Description:
// Sorting proxy model for memory blocks.
//-----------------------------------------------------------------------------

#ifndef MEMORYBLOCKFILTER_H
#define MEMORYBLOCKFILTER_H

#include <editors/ComponentEditor/memoryMaps/ExpressionProxyModel.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class MemoryBlockInterface;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! Sorting proxy model for ports.
//-----------------------------------------------------------------------------
class MemoryBlockFilter : public ExpressionProxyModel
{
    Q_OBJECT

public:

    /*!
     *  Constructor.
     *
     *      @param [in] parser          The expression parser.
     *      @param [in] blockInterface  Interface for accessing memory blocks.
     *      @param [in] parent          The parent object.
     */
    MemoryBlockFilter(QSharedPointer<ExpressionParser> parser, MemoryBlockInterface* blockInterface,
        QObject* parent = 0);

	 /*!
      *  Destructor.
      */
    virtual ~MemoryBlockFilter() = default;

    // No copying. No assignments.
    MemoryBlockFilter(MemoryBlockFilter const& rhs) = delete;
    MemoryBlockFilter& operator=(MemoryBlockFilter const& rhs) = delete;

protected:

    /*!
	 *  Implementation of the row filtering.
	 *
	 *      @param [in] source_row      The row to check for filtering.
     *      @param [in] source_parent   The parent index of the row.
     *
     *      @return True, if the row passes the filters, otherwise false.
	 */
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing the memory blocks.
    MemoryBlockInterface* blockInterface_;
};

#endif // MEMORYBLOCKFILTER_H
