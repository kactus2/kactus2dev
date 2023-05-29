//-----------------------------------------------------------------------------
// File: CommonInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2020
//
// Description:
// Common functions for interfaces.
//-----------------------------------------------------------------------------

#ifndef COMMONINTERFACE_H
#define COMMONINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <string>
#include <vector>
#include <QString>

//-----------------------------------------------------------------------------
//! Common functions for interfaces.
//-----------------------------------------------------------------------------
class KACTUS2_API CommonInterface
{
public:

    /*!
     *  The constructor.
     */
    CommonInterface() = default;

	/*!
     *  The destructor.
     */
    virtual ~CommonInterface() = default;

    /*!
     *  Get the names of the available items.
     *
     *      @return Names of the available items.
     */
    virtual std::vector<std::string> getItemNames() const = 0;

    /*!
     *  Get the number of available items.
     *
     *      @return Number of available items.
     */
    virtual int itemCount() const = 0;

    /*!
     *  Validates the contained items.
     *
     *      @return True, if all the items are valid, false otherwise.
     */
    virtual bool validateItems() const = 0;

    //! No copying. No assignment.
    CommonInterface(const CommonInterface& other) = delete;
    CommonInterface& operator=(const CommonInterface& other) = delete;

protected:

    /*!
     *  Transform the selected name to a unique item name.
     *
     *      @param [in] newName     The selected name.
     *
     *      @return A unique name derived from the selected name.
     */
    std::string getUniqueName(std::string_view newName, std::string_view itemTypeName) const;

    /*!
     *  Check if the new name is different from the current name.
     *
     *      @param [in] newName     The new name.
     *      @param [in] oldName     The current name.
     *
     *      @return True, if the new name is different, false otherwise.
     */
    bool nameHasChanged(std::string_view newName, std::string_view oldName) const;

private:

    /*!
     *  Check if the selected name is unique.
     *
     *      @param [in] name    The selected name.
     *
     *      @return True, if the selected name is unique, false otherwise.
     */
    bool nameIsUnique(std::string_view name) const;
};

#endif // COMMONINTERFACE_H
