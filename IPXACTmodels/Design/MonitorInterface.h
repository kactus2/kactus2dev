//-----------------------------------------------------------------------------
// File: MonitorInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.08.2015
//
// Description:
// Describes the ipxact:MonitorInterface element in an IP-XACT document.
//-----------------------------------------------------------------------------

#ifndef MONITORINTERFACE_H
#define MONITORINTERFACE_H

#include "HierInterface.h"

#include <IPXACTmodels/ipxactmodels_global.h>

//-----------------------------------------------------------------------------
// Ipxact:MonitorInterface element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MonitorInterface : public HierInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] componentRef    The name of the referenced component instance.
     *      @param [in] busRef          The name of the referenced bus interface.
     */
    MonitorInterface(std::string componentRef = std::string(), std::string busRef = std::string());

    /*!
     *  Copy constructor.
     */
    MonitorInterface(const MonitorInterface& other);

    /*!
     *  The destructor.
     */
    ~MonitorInterface() final {};

    /*!
     *  Assignment operator.
     */
    MonitorInterface& operator=(const MonitorInterface& other);

    /*!
     *  Comparison operator.
     */
    bool operator==(const MonitorInterface& other) const;

    /*!
     *  The operator !=.
     */
    bool operator!=(const MonitorInterface& other) const;

    /*!
     *  The operator <.
     */
    bool operator<(const MonitorInterface& other) const;

    /*!
     *  Checks if the interface referenced a given instance and a bus interface.
     *
     *      @param [in] instanceName        The name of the component instance to compare to.
     *      @param [in] busInterfaceName    The name of the bus interface to compare to.
     *
     *      @return True, if the given instance and bus interface are reference by the interface.
     */
    bool references(std::string const& instanceName, std::string const& busInterfaceName) const;

    /*!
     *  Get the component reference.
     *
     *      @return The name of the referenced component
     */
    std::string getComponentReference() const;

    /*!
     *  Set the component reference.
     *
     *      @param [in] newComponentReference   The new component reference.
     */
    void setComponentReference(std::string const& newComponentReference);

    /*!
     *  Get the path.
     *
     *      @return The path.
     */
    std::string getPath() const;

    /*!
     *  Set the path.
     *
     *      @param [in] newPath     The new path.
     */
    void setPath(std::string const& newPath);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Component reference.
    std::string componentReference_;

    //! The path.
    std::string path_;
};
#endif // MONITORINTERFACE_H
