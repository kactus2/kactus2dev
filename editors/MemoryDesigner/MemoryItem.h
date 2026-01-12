//-----------------------------------------------------------------------------
// File: MemoryItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.08.2016
//
// Description:
// A memory in a connectivity graph.
//-----------------------------------------------------------------------------

#ifndef MEMORYITEM_H
#define MEMORYITEM_H

#include <IPXACTmodels/common/AccessTypes.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QString>
#include <QSharedPointer>
#include <QVector>

//-----------------------------------------------------------------------------
//! A memory in a connectivity graph.
//-----------------------------------------------------------------------------
class MemoryItem 
{
public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] name   The name of the memory.
     *    @param [in] type   The type of the memory.
	 */
	MemoryItem(QString const& name, QString const& type);

    /*!
     *  Gets the name of the memory.
     *
     *    @return The name of the memory.
     */
    QString getName() const;

    /*!
     *  Gets the type of the memory e.g. address space or register.
     *
     *    @return The type of the memory.
     */
    QString getType() const;

    /*!
     *  Sets a display name for the memory.
     *
     *    @param [in] displayName   The display name to set.
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Gets the display name of the memory.
     *
     *    @return The display name of the memory.
     */
    QString getDisplayName() const;

    /*!
     *  Set the description
     *
     *    @param [in] newDescription    The new description.
     */
    void setDescription(QString const& newDescription);

    /*!
     *  Get the description
     *
     *    @return The description
     */
    QString getDescription() const;

    /*!
     *  Sets an identifier for the memory.
     *
     *    @param [in] identifier   The identifier to set.
     */
    void setIdentifier(QString const& identifier);
    
    /*!
     *  Gets the memory identifier.
     *
     *    @return The identifier for the memory.
     */
    QString getIdentifier() const;

    /*!
     *  Gets the number of addressable unit bits in the memory.
     *
     *    @return The number addressable unit bits in the memory.
     */
    QString getAUB() const;

    /*!
     *  Sets the number of addressable unit bits in the memory.
     *
     *    @param [in] aub   The number of bits in an addressable unit.
     */
    void setAUB(QString const& aub);

    /*!
     *  Sets the address for the memory.
     *
     *    @param [in] address   The memory address to set.
     */
    void setAddress(QString const& address);

    /*!
     *  Gets the memory address.
     *
     *    @return The address for the memory.
     */
    QString getAddress() const;

    /*!
     *  Set the formatted address expression.
     *
     *    @param [in] newFormattedAddressExpression     The new formatted address expression.
     */
    void setFormattedAddressExpression(QString const& newFormattedAddressExpression);

    /*!
     *  Get the formatted address expression.
     *
     *    @return The formatted address expression.
     */
    QString getFormattedAddressExpression() const;

    /*!
     * Sets the range for the memory.
     *
     *    @param [in] range   The range to set.
     */
    void setRange(QString const& range);

    /*!
     *  Gets the range for the memory.
     *
     *    @return The range for the memory.
     */
    QString getRange() const;

    /*!
     *  Set the formatted range expression.
     *
     *    @param [in] newFormattedRangeExpression   The new formatted range expression.
     */
    void setFormattedRangeExpression(QString const& newRangeExpression);

    /*!
     *  Get the formatted range expression.
     *
     *    @return The formatted range expression.
     */
    QString getFormattedRangeExpression() const;

    /*!
     *  Sets the width for the memory.
     *
     *    @param [in] width   The memory width to set.
     */
    void setWidth(QString const& width);

    /*!
     *  Gets the memory width in bits.
     *
     *    @return The width of the memory.
     */
    QString getWidth() const;

    /*!
     *  Set the formatted width expression.
     *
     *    @param [in] newFormattedWidthExpression   The new formatted width expression.
     */
    void setFormattedWidthExpression(QString const& newWidthExpression);

    /*!
     *  Get the formatted width expression.
     *
     *    @return The formatted width expression.
     */
    QString getFormattedWidthExpression() const;

	/*!
	 *  Set the access value.
	 *
	 *    @param [in] newAccess     The new access value.
	 */
	void setAccess(AccessTypes::Access const& newAccess);

    /*!
     *  Get the access value.
     *
     *    @return The access value.
     */
    AccessTypes::Access getAccess() const;

    /*!
     *  Sets the size for the memory.
     *
     *    @param [in] size    The size to set.
     */
    void setSize(QString const& size);

    /*!
     *  Gets the size for the memory.
     *
     *    @return The memory size.
     */
    QString getSize() const;

    /*!
     *  Sets the offset for the memory.
     *
     *    @param [in] offset   The offset to set.
     */
    void setOffset(QString const& offset);

    /*!
     *  Gets the offset for the memory.
     *
     *    @return The memory offset.
     */
    QString getOffset() const;

    /*!
     *  Set the formatted offset expression.
     *
     *    @param [in] newFormattedOffsetExpression  The new formatted offset expression.
     */
    void setFormattedOffsetExpression(QString const& newOffsetExpression);

    /*!
     *  Get the formatted offset expression.
     *
     *    @return The formatted offset expression.
     */
    QString getFormattedOffsetExpression() const;

    /*!
     *  Adds a sub-element for the memory.
     *
     *    @param [in] child   The sub-element to add.
     */
    void addChild(QSharedPointer<MemoryItem> child);

    /*!
     *  Gets the sub-elements for the memory.
     *
     *    @return The sub-elements for the memory.
     */
    QVector<QSharedPointer<MemoryItem> > getChildItems() const;

    /*!
     *  Set the dimension of the memory item.
     *
     *    @param [in] dimension    The new dimension value.
     */
    void setDimension(QString const& dimension);

    /*!
     *  Gets the dimension of the memory. Only used for registers.
     *
     *    @return The dimension of the memory.
     */
    QString getDimension() const;

    /*!
     *  Set the usage for the memory item.
     *
     *    @param [in] newUsage    The new usage value.
     */
    void setUsage(General::Usage const& newUsage);

    /*!
     *  Get the usage of the memory item.
     *
     *    @return The usage value.
     */
    General::Usage getUsage() const;

    /*!
     *  Set the value for the memory item.
     *
     *    @param [in] newValue    The new value.
     */
    void setValue(QString const& newValue);

    /*!
     *  Get the value of the memory item.
     *
     *    @return The value.
     */
    QString getValue() const;

    /*!
     *  Get the reset value of the memory item.
     *
     *    @return The reset value of the memory item.
     */
    QString getResetValue() const;

    /*!
     *  Set a new reset value for the memory item.
     *
     *    @param [in] newResetValue   The new reset value.
     */
    void setResetValue(QString const& newResetValue);

    /*!
     *  Get the reset mask of the memory item.
     *
     *    @return The reset mask of the memory item.
     */
    QString getResetMask() const;

    /*!
     *  Set a new reset mask for the memory item.
     *
     *    @param [in] newResetMask    The new reset mask.
     */
    void setResetMask(QString const& newResetMask);

    /*!
     *  Get the is present of the memory item.
     *
     *    @return The is present of the memory item.
     */
    QString getIsPresent() const;

    /*!
     *  Set a new is present for the memory item.
     *
     *    @param [in] newIsPresent    The new is present.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
     *  Set a new initiator reference for the memory item.
     *
     *    @param [in] newReference    The new initiator reference.
     */
    void setInitiatorReference(QString const& newReference);

    /*!
     *  Get the initiator reference of the memory item.
     */
    QString getInitiatorReference() const;

    /*!
     *  Set a new segment reference for the memory item.
     *
     *    @param [in] newReference    The new segment reference.
     */
    void setSegmentReference(QString const& newReference);

    /*!
     *  Get the segment reference of the memory item.
     */
    QString getSegmentReference() const;

private:
    //! The name of the memory element.
    QString name_ = "";

    //! The display name of the memory element.
    QString displayName_ = "";

    //! Description of the memory element.
    QString description_ = "";

    //! The type of the memory element.
    QString type_ = "";

    //! The identifier for the memory.
    QString identifier_ = "";

    //! The number of addressable unit bits in the memory.
    QString aub_ = "";

    //! The address for the memory.
    QString address_ = "";

    //! Formatted address expression for the memory.
    QString formattedAddressExpression_ = "";

    //! The range for the memory.
    QString range_ = "";

	//! Formatted range expression for the memory.
    QString formattedRangeExpressin_ = "";

    //! The bit width for the memory.
    QString width_ = "";

	//! Formatted width expression for the memory.
    QString formattedWidthExpression_ = "";

    //! Access value of the memory.
    AccessTypes::Access access_ = AccessTypes::ACCESS_COUNT;

    //! The size of the memory.
    QString size_ = "";

    //! The offset of the memory.
    QString offset_ = "";

	//! Formatted offset expression for the memory.
    QString formattedOffsetExpression_ = "";

    //! Value of the memory item.
    QString value_ = "";

    //! The dimension for the memory.
    QString dimension_ = "";

    //! The usage value of the memory item.
    General::Usage usage_{ General::USAGE_COUNT };

    //! Reset value of the memory item.
    QString resetValue_ = "";

    //! Reset mask of the memory item.
    QString resetMask_ = "";

    //! Presence of the memory item.
    QString isPresent_{ "1" };

    //! The initiator reference.
    QString initiatorReference_ = "";

    //! The segment reference.
    QString segmentReference_ = "";

    //! The sub-elements of the memory.
    QVector<QSharedPointer<MemoryItem> > childItems_;
};

#endif // MEMORYITEM_H
