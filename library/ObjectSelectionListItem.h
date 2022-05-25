//-----------------------------------------------------------------------------
// File: ObjectSelectionListItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.07.2017
//
// Description:
// List item for VLNVs and files.
//-----------------------------------------------------------------------------

#ifndef OBJECTSELECTIONLISTITEM_H
#define OBJECTSELECTIONLISTITEM_H

#include <IPXACTmodels/common/VLNV.h>

#include <QIcon>
#include <QListWidgetItem>

//-----------------------------------------------------------------------------
//! List item for VLNVs and files.
//-----------------------------------------------------------------------------
class ObjectSelectionListItem : public QListWidgetItem
{

public:

    //! Type of the item.
    enum ItemType
    {
        VLNVOJBECT = 0,     // VLNV object item.
        FILE                // file item.
    };

	/*!
     *  The constructor.
	 *
     *      @param [in] itemIcon    Icon used by the item.
     *      @param [in] itemPath    File path of the item.
     *      @param [in] isLocked    The lock status of the item.
     *      @param [in] vlnv        VLNV associated with the item.
	 *
     */
    ObjectSelectionListItem(QIcon itemIcon, QString const& itemPath, bool isLocked, VLNV vlnv = VLNV());

	/*!
     *  The destructor.
     */
    virtual ~ObjectSelectionListItem();

    /*!
     *  Get the VLNV associated with the item.
     *
     *      @return The VLNV of the item.
     */
    VLNV getVLNV() const;

    /*!
     *  Get the file path of the item.
     *
     *      @return The file path of the item.
     */
    QString getPath() const;

    /*!
     *  Get the item type.
     *
     *      @return The type of the item.
     */
    ObjectSelectionListItem::ItemType getType() const;

    /*!
     *  Check if the item is locked.
     *
     *      @return True, if the item is locked, false otherwise.
     */
    bool itemIsLocked() const;

private:

    //! No copying. No assignment.
    ObjectSelectionListItem(const ObjectSelectionListItem& other);
    ObjectSelectionListItem& operator=(const ObjectSelectionListItem& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! VLNV associated with the item.
    VLNV itemVlnv_;

    //! File path of the item.
    QString itemPath_;

    //! Type of the item.
    ObjectSelectionListItem::ItemType type_;

    //! Lock status of the item.
    bool locked_;
};

#endif // OBJECTSELECTIONLISTITEM_H
