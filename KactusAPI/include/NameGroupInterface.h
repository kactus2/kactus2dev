//-----------------------------------------------------------------------------
// File: NameGroupInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2020
//
// Description:
// Interface for name groups.
//-----------------------------------------------------------------------------

#ifndef NAMEGROUPINTERFACE_H
#define NAMEGROUPINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <IPXACTmodels/common/NameGroup.h>

#include "CommonInterface.h"

#include <QString>

//-----------------------------------------------------------------------------
//! Interface for name groups.
//-----------------------------------------------------------------------------
class KACTUS2_API NameGroupInterface : public CommonInterface
{
public:

    /*!
     *  The constructor.
     */
    NameGroupInterface();

	/*!
     *  The destructor.
     */
    virtual ~NameGroupInterface() = default;
    
    /*!
     *  Get index of the selected item.
     *
     *    @param [in] itemName    Name of the selected item.
     *
     *    @return Index of the selected item.
     */
    virtual int getItemIndex(std::string const& itemName) const = 0;

    /*!
     *  Get name of the indexed item.
     *
     *    @param [in] itemIndex   Index of the selected item.
     *
     *    @return Name of the selected item.
     */
    virtual std::string getIndexedItemName(int itemIndex) const = 0;

    /*!
     *  Set a new name for the selected item.
     *
     *    @param [in] currentName     Name of the selected item.
     *    @param [in] newName         New name for the item.
     *
     *    @return True, if successful, false otherwise.
     */
    virtual bool setName(std::string const& currentName, std::string const& newName) = 0;
    
    /*!
     *	Get the display name of the selected item.
     *  
     *    @param [in] itemName	The name of the item whose display name to get.
     *		
     * 		@return The item display name.
     */
    virtual std::string getDisplayName(std::string const& itemName) const;

    /*!
     *	Set the display name of the selected item.
     *  
     *    @param [in] itemName	Description
     *    @param [in] newDisplayName	Description
     *		
     * 		@return True, if display name was set successfully, otherwise false.
     */
    virtual bool setDisplayName(std::string const& itemName, std::string const& newDisplayName) const;

    /*!
     *  Get the description of the selected item.
     *
     *    @param [in] itemName    Name of the selected item.
     *
     *    @return Description of the selected item.
     */
    virtual std::string getDescription(std::string const& itemName) const;

    /*!
     *  Set a new description for the selected item.
     *
     *    @param [in] itemName        Name of the selected item.
     *    @param [in] newDescription  New description.
     *
     *    @return True, if successful, false otherwise.
     */
    virtual bool setDescription(std::string const& itemName, std::string const& newDescription);

    /*!
     *	Get the short description of an item.
     *  
     *    @param [in] itemName	Name of the selected item.
     *		
     * 		@return Short description of the selected item.
     */
    virtual std::string getShortDescription(std::string const& itemName) const;
    
    /*!
     *	Set a new short description for the selected item.
     *  
     *    @param [in] itemName	Description
     *    @param [in] newShortDescription	Description
     *		
     * 		@return True, if successful, otherwise false.
     */
    bool setShortDescription(std::string const& itemName, std::string const& newShortDescription) const;

    /*!
     *  Check if the selected item has a valid name.
     *
     *    @param [in] itemName    Name of the selected item.
     *
     *    @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const = 0;

    //! No copying. No assignment.
    NameGroupInterface(const NameGroupInterface& other) = delete;
    NameGroupInterface& operator=(const NameGroupInterface& other) = delete;

protected:

    /*!
     *  Get the item with the selected name, sliced to a NameGroup.
     *
     *    @param [in] itemName    Name of the selected item.
     *
     *    @return The sliced item with the selected name.
     */
    virtual QSharedPointer<NameGroup> getItem(std::string const& itemName) const = 0;
};

#endif // NAMEGROUPINTERFACE_H
