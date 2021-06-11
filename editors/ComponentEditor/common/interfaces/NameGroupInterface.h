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

#include <common/Global.h>

#include <editors/ComponentEditor/common/interfaces/CommonInterface.h>

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
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Index of the selected item.
     */
    virtual int getItemIndex(std::string const& itemName) const = 0;

    /*!
     *  Get name of the indexed item.
     *
     *      @param [in] itemIndex   Index of the selected item.
     *
     *      @return Name of the selected item.
     */
    virtual std::string getIndexedItemName(int const& itemIndex) const = 0;

    /*!
     *  Set a new name for the selected item.
     *
     *      @param [in] currentName     Name of the selected item.
     *      @param [in] newName         New name for the item.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(std::string const& currentName, std::string const& newName) = 0;

    /*!
     *  Get the description of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Description of the selected item.
     */
    virtual std::string getDescription(std::string const& itemName) const = 0;

    /*!
     *  Set a new description for the selected item.
     *
     *      @param [in] itemName        Name of the selected item.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(std::string const& itemName, std::string const& newDescription) = 0;

    /*!
     *  Check if the selected item has a valid name.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(std::string const& itemName) const = 0;

    //! No copying. No assignment.
    NameGroupInterface(const NameGroupInterface& other) = delete;
    NameGroupInterface& operator=(const NameGroupInterface& other) = delete;
};

#endif // NAMEGROUPINTERFACE_H
