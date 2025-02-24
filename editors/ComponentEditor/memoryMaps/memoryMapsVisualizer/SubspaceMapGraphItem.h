//-----------------------------------------------------------------------------
// File: SubspaceMapGraphItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2022
//
// Description:
// The graphical item that represents one subspace map.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAPGRAPHITEM_H
#define SUBSPACEMAPGRAPHITEM_H

#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/MemoryBlockGraphItem.h>
#include <KactusAPI/include/SubspaceMapInterface.h>

#include <QGraphicsItem>
#include <QSharedPointer>

#include <QColor>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphical item that represents one subspace map.
//-----------------------------------------------------------------------------
class SubspaceMapGraphItem : public MemoryBlockGraphItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
     *    @param [in] submapInterface     Interface for accessing subspace maps.
     *    @param [in] subspaceName        Name of the represented subspace map.
     *    @param [in] expressionParser    The used expression parser.
     *    @param [in] parent              The parent graph item.
	 */
    SubspaceMapGraphItem(SubspaceMapInterface* submapInterface,
        QString const& subspaceName,
        QSharedPointer<ExpressionParser> expressionParser,
		QGraphicsItem *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~SubspaceMapGraphItem() = default;

    //! No copying. No assignment.
    SubspaceMapGraphItem(const SubspaceMapGraphItem& other) = delete;
    SubspaceMapGraphItem& operator=(const SubspaceMapGraphItem& other) = delete;

	/*!
     *  Get the offset of the item. 
	 *
	 *    @return int The offset of the item from the parent item's base address.
	 */
	virtual quint64 getOffset() const override final;

	/*!
     *  Get the bit width of the item.
	 * 
	 *    @return The bit width of the item.
	 */
	virtual int getBitWidth() const override final;
           
    /*!
     *  Checks if the item is to be used in the visualization.
     *
     *    @return True, if the item should be used, otherwise false.
     */
    virtual bool isPresent() const override final;

public slots:

    /*!
     *  Handle the name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void onNameChanged(QString const& oldName, QString const& newName);

private:

    /*!
     *  Get the name of the memory block.
     *
     *    @return Name of the memory block.
     */
    virtual QString getName() const override final;

    /*!
     *  Get the range of the memory block.
     *
     *    @return Range of the memory block.
     */
    virtual quint64 getRange() const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing subspace maps.
    SubspaceMapInterface* submapInterface_;
    
    //! Name of the represented subspace map.
    std::string subspaceName_;
};

#endif // SUBSPACEMAPGRAPHITEM_H
