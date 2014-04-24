//-----------------------------------------------------------------------------
// File: PortMapsTreeItem
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 7.10.2013
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef PORTMAPSTREEITEM_H
#define PORTMAPSTREEITEM_H

#include <QObject>
#include <QString>
#include <QList>
#include <QSharedPointer>

#include <IPXACTmodels/generaldeclarations.h>

class AbstractionDefinition;
class BusInterface;
class Component;



//-----------------------------------------------------------------------------
//! Base class for port maps items.
//-----------------------------------------------------------------------------
class PortMapsTreeItem : public QObject
{
    Q_OBJECT

public:

    //-----------------------------------------------------------------------------
    //! Item type enumeration.
    //-----------------------------------------------------------------------------
    enum ItemType
    {
        ITEM_TYPE_ROOT = 0,             //!< For the root item.
        ITEM_LOGICAL_PORT,              //!< For logical ports.
        ITEM_BIT_MAP,                   //!< For physical mappings.
        ITEM_TYPE_COUNT
    };

    /*!
     *  Default constructor which creates a root item.
     */
    PortMapsTreeItem();

    /*!
     *  Default constructor which creates a root item.
     */
    PortMapsTreeItem(PortMapsTreeItem* parent, QSharedPointer<Component> component, 
        QString const& name, ItemType type);

    /*!
     *  Destructor.
     */
    virtual ~PortMapsTreeItem();

    /*!
     *  Updates the item. The default implementation does nothing.
     *
     */
    virtual void refresh();

    /*!
     *  Gets the item data for a given section.
     *
     *      @param [in] section   The section whose data to get.
     *
     *      @return The data in section.
     */
    virtual QVariant data(int section) const;

    /*!
     *  Checks the validity of the item. Default implementation checks all child items.
     *
     *      @return True, if all childs are valid, otherwise false.
     */
    virtual bool isValid() const;

    /*!
     *  Checks the validity of the item. Default implementation checks all child items.
     *
     *      @param [inout] errorList   The list to add the possible error messages to.
     *
     *      @return True, if all childs are valid, otherwise false.
     */
    virtual bool isValid(QStringList& errorList) const;


    /*!
     *  Inserts an item.
     *
     *      @param [in] item The item to insert.
     */
    virtual void addChild(PortMapsTreeItem* item);

    /*!
     *  Removes an item.
     *
     *      @param [in] item The item to remove.
     */
    virtual void removeChild(PortMapsTreeItem* item);

    /*!
     *  Returns the child at the given index.
     *
     *      @param [in] index The index of the child to retrieve.
     */
    PortMapsTreeItem* getChild(int index);

    /*!
     *  Returns the child at the given index.
     *
     *      @param [in] index The index of the child to retrieve.
     */
    PortMapsTreeItem const* getChild(int index) const;

    /*!
     *  Returns the number of child items.
     */
    int getChildCount() const;

    /*!
     *  Returns the parent item.
     */
    PortMapsTreeItem* getParent() const;

    /*!
     *  Returns the index of this item.
     */
    int getIndex() const;

    /*!
     *  Returns the item type.
     */
    ItemType getType() const;

    /*!
     *  Gets the name of the item.
     *
     *      @return The name of the item.
     */
    QString getName() const;

protected:

    //! The component being edited.
    QSharedPointer<Component> component_;

private:
    // Disable copying.
    PortMapsTreeItem(PortMapsTreeItem const& rhs);
    PortMapsTreeItem& operator=(PortMapsTreeItem const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent item.
    PortMapsTreeItem* parent_;
    
    //! The item type.
    ItemType type_;

    //! The name of the item.
    QString name_;

    //! The child items.
    QList<PortMapsTreeItem*> children_;
};

#endif // PORTMAPSTREEITEM_H
