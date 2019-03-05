//-----------------------------------------------------------------------------
// File: AutoConnectorItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.02.2019
//
// Description:
// Item used by component item auto connector.
//-----------------------------------------------------------------------------

#ifndef AUTOCONNECTORITEM_H
#define AUTOCONNECTORITEM_H

#include <QString>

//-----------------------------------------------------------------------------
//! Item used by component item auto connector.
//-----------------------------------------------------------------------------
class AutoConnectorItem
{

public:

    //! Item type.
    enum ItemType
    {
        PORT,
        BUS_INTERFACE,
        INVALID
    };

    //! Container type.
    enum ContainerType
    {
        TOP_COMPONENT,
        COMPONENT_ITEM,
        INVALID_CONTAINER
    };

    /*!
     *  The constructor.
     *
     *      @param [in] itemName            Name of the connected item.
     *      @param [in] componentItemName   Name of the containing component item.
     *      @param [in] itemType            Type of the item.
     *      @param [in] containerType       Type of the container.
     */
    AutoConnectorItem(QString const& itemName, QString const& componentItemName,
        AutoConnectorItem::ItemType itemType,
        AutoConnectorItem::ContainerType containerType = AutoConnectorItem::ContainerType::COMPONENT_ITEM);

    /*!
     *  Copy constructor.
     */
    AutoConnectorItem(const AutoConnectorItem &other);

    /*!
     *  Assignment operator.
     */
    AutoConnectorItem &operator=(const AutoConnectorItem &other);

    /*!
     *  The destructor.
     */
    ~AutoConnectorItem();

    /*!
     *  Get the name.
     *
     *      @return Name of the item.
     */
    QString getName() const;

    /*!
     *  Set a new name.
     *
     *      @param [in] newName     New name for the item.
     */
    void setName(QString const& newName);

    /*!
     *  Get the name of the containing component item.
     *
     *      @return The name of the containing component item.
     */
    QString getContainingItem() const;

    /*!
     *  Set the name of the containing component item.
     *
     *      @param [in] newComponentItem    New name for the containing component item.
     */
    void setContainingItem(QString const& newComponentItem);

    /*!
     *  Get the type.
     *
     *      @return Type of the item.
     */
    AutoConnectorItem::ItemType getItemType() const;

    /*!
     *  Set the type for the item.
     *
     *      @param [in] newItemType     New type for the item.
     */
    void setNewItemType(AutoConnectorItem::ItemType newItemType);

    /*!
     *  Get the container type.
     *
     *      @return Type of the container.
     */
    AutoConnectorItem::ContainerType getContainterType() const;

    /*!
     *  Set the type for the container.
     *
     *      @param [in] newContainerType    New type for the container.
     */
    void setNewContainerType(AutoConnectorItem::ContainerType newContainerType);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Name of the item.
    QString itemName_;

    //! Name of the containing component item.
    QString containingItemName_;

    //! Item type.
    AutoConnectorItem::ItemType itemType_;

    //! Container type.
    AutoConnectorItem::ContainerType containterType_;
};

#endif // AUTOCONNECTORITEM_H
