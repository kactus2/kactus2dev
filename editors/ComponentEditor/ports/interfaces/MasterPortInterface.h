//-----------------------------------------------------------------------------
// File: MasterPortInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.12.2020
//
// Description:
// Master interface for editing component ports and port abstractions.
//-----------------------------------------------------------------------------

#ifndef MASTERPORTINTERFACE_H
#define MASTERPORTINTERFACE_H

#include <editors/ComponentEditor/common/interfaces/NameGroupInterface.h>

#include <IPXACTmodels/common/DirectionTypes.h>

//-----------------------------------------------------------------------------
//! Master interface for editing component ports and port abstractions.
//-----------------------------------------------------------------------------
class MasterPortInterface : public NameGroupInterface
{
public:

    /*!
     *  The constructor.
     */
    MasterPortInterface();

    /*!
     *  The destructor.
     */
    virtual ~MasterPortInterface() = default;

    /*!
     *  Add a wire port.
     *
     *      @param [in] newPortName     Name of the new port.
     */
    virtual void addWirePort(std::string const& newPortName = std::string("")) = 0;

    /*!
     *  Add a transactional port.
     *
     *      @param [in] newPortName     Name of the new port.
     */
    virtual void addTransactionalPort(std::string const& newPortName = std::string("")) = 0;

    /*!
     *  Check if the selected port is a wire.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the selected port is a wire, false otherwise.
     */
    virtual bool portIsWire(std::string const& portName) const = 0;

    /*!
     *  Check if the selected port is transactional.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the selected port is transactional, false otherwise.
     */
    virtual bool portIsTransactional(std::string const& portName) const = 0;

    /*!
     *  Get the icon path of a missing port.
     *
     *      @return Icon path for the selected port.
     */
    std::string getIconPathForMissingPort() const;

    /*!
     *  Get the icon path of the selected direction.
     *
     *      @param [in] direction   Direction of the selected port.
     *
     *      @return Icon path for the selected direction.
     */
    std::string getIconPathForDirection(DirectionTypes::Direction direction) const;

    /*!
     *  Get the icon path of the selected initiative.
     *
     *      @param [in] initiative  The selected initiative.
     *
     *      @return Icon path for the selected initiative.
     */
    std::string getIconPathForInitiative(QString const& initiative) const;

    //! No copying. No assignment.
    MasterPortInterface(const MasterPortInterface& other) = delete;
    MasterPortInterface& operator=(const MasterPortInterface& other) = delete;
};

#endif // MASTERPORTINTERFACE_H
