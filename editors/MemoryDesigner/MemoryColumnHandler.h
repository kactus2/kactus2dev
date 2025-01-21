//-----------------------------------------------------------------------------
// File: MemoryColumnHandler.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.01.2017
//
// Description:
// Constructs the columns for the memory design diagram.
//-----------------------------------------------------------------------------

#ifndef MEMORYCOLUMNHANDLER_H
#define MEMORYCOLUMNHANDLER_H

#include <common/graphicsItems/GraphicsColumnLayout.h>

class MemoryColumn;

//-----------------------------------------------------------------------------
//! Constructs the columns for the memory design diagram.
//-----------------------------------------------------------------------------
class MemoryColumnHandler
{

public:

    /*!
     *  The constructor.
     *
     *    @param [in] layout  Layout for the memory columns.
     */    
    MemoryColumnHandler(QSharedPointer<GraphicsColumnLayout> layout);

    /*!
     *  The destructor.
     */
    ~MemoryColumnHandler();

    /*!
     *  Set a new column width for memory map columns.
     *
     *    @param [in] newMapColumnWidth   The new column width.
     */
    void setNewMemoryMapColumnWidth(int newMapColumnWidth);

    /*!
     *  Get the width of the memory map columns.
     *
     *    @return Width of the memory map columns.
     */
    int getMemoryMapColumnWidth() const;

    /*!
     *  Set a new layout for the memory columns.
     *
     *    @param [in] newLayout   The new layout for the memory columns.
     */
    void setNewLayout(QSharedPointer<GraphicsColumnLayout> newLayout);

    /*!
     *  Get the memory columns.
     *
     *    @return The memory columns in the layout.
     */
    QVector<MemoryColumn*> getMemoryColumns() const;

    /*!
     *  Get the address space columns.
     *
     *    @return The address space columns in the layout.
     */
    QVector<MemoryColumn*> getAddressSpaceColumns() const;

    /*!
     *  Get the memory map columns.
     *
     *    @return The memory map columns in the layout.
     */
    QVector<MemoryColumn*> getMemoryMapColumns() const;

    /*!
     *  Get the memory map overlap columns.
     *
     *    @return The memory map overlap columns in the layout.
     */
    QVector<MemoryColumn*> getMapOverlapColumns() const;

    /*!
     *  Get the column located at the given position.
     *
     *    @param [in] position    Position of the selected column.
     *
     *    @return Column located at the given position, or 0, if no column was found.
     */
    MemoryColumn* findColumnAtPosition(QPointF position) const;

    /*!
     *  Remove the selected column from the layout.
     *
     *    @param [in] selectedColumn  The selected column.
     */
    void removeColumnFromLayout(MemoryColumn* selectedColumn);

    /*!
     *  Create the initial memory columns.
     */
    void createInitialColumns();

    /*!
     *  Create an address space column.
     *
     *    @return The created address space column.
     */
    MemoryColumn* createAddressSpaceColumn();

    /*!
     *  Create a new memory overlap column.
     *
     *    @return The created memory overlap column.
     */
    MemoryColumn* createMemoryOverlapColumn();

    /*!
     *  Get the selected memory column.
     *
     *    @param [in] columnName  Name of the selected memory column.
     *
     *    @return The found memory column.
     */
    MemoryColumn* findColumnByName(QString const& columnName) const;

    /*!
     *  Get the specified columns.
     *
     *    @param [in] columnSpecification     Identifier for the wanted columns.
     *
     *    @return A vector containing the specified columns.
     */
    QVector<MemoryColumn*> getSpecifiedColumns(QString const& columnSpecification) const;

    /*!
     *  Change the column widths by the selected amount.
     *
     *    @param [in] deltaWidth  The selected width change.
     */
    void changeColumnWidths(qreal deltaWidth);

private:
    // Disable copying.
    MemoryColumnHandler(MemoryColumnHandler const& rhs);
    MemoryColumnHandler& operator=(MemoryColumnHandler const& rhs);

    /*!
     *  Create the initial memory map columns.
     */
    void createMemoryColumns();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Width of the address space graphics item columns.
    int spaceColumnWidth_;

    //! Width of the memory map graphics item columns.
    int memoryMapColumnWidth_;

    //! Layout for the memory columns.
    QSharedPointer<GraphicsColumnLayout> layout_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYCOLUMNHANDLER_H
