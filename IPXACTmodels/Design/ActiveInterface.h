//-----------------------------------------------------------------------------
// File: ActiveInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.08.2015
//
// Description:
// Describes the ipxact:activeInterface element in an IP-XACT document.
//-----------------------------------------------------------------------------

#ifndef ACTIVEINTERFACE_H
#define ACTIVEINTERFACE_H

#include "HierInterface.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QStringList>

//-----------------------------------------------------------------------------
// Ipxact:activeInterface element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ActiveInterface : public HierInterface
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] componentRef    The name of the referenced component instance.
     *      @param [in] busRef          The name of the referenced bus interface.
     */
    ActiveInterface(std::string const& componentRef = std::string(), std::string const& busRef = std::string());

    /*!
     *  Copy constructor.
     */
    ActiveInterface(const ActiveInterface& other);
    
    /*!
     *  The destructor.
     */
    ~ActiveInterface() final = default;

    /*!
     *  Assignment operator.
     */
    ActiveInterface& operator=(const ActiveInterface& other);

    /*!
     *  Comparison operator.
     *
     *      @return True, if the interfaces refer the same component and bus interface, otherwise false.
     */
    bool operator==(const ActiveInterface& other) const;

    /*!
     *  The != operator.
     */
    bool operator!=(const ActiveInterface& other);

    /*!
     *  The operator <.
     *
     *      @return The operator uses QString to compare instance names and bus interface names.
     */
    bool operator<(const ActiveInterface& other);

    /*!
     *  Checks if the interface referenced a given instance and a bus interface.
     *
     *      @param [in] instanceName        The name of the component instance to compare to.
     *      @param [in] busInterfaceName    The name of the bus interface to compare to.
     *
     *      @return True, if the given instance and bus interface are reference by the interface.
     */
    bool references(std::string const& instanceName, std::string const& busInterfaceName);

    /*!
     *  Get the referenced component.
     *
     *      @return The name of the referenced component.
     */
    std::string getComponentReference() const;

    /*!
     *  Set a new component referenced.
     *
     *      @param [in] newComponentReference   The new component reference.
     */
    void setComponentReference(std::string const& newComponentReference);

    /*!
     *  Get a list of excluded ports.
     *
     *      @return Pointer to a list containing the excluded ports.
     */
    QSharedPointer<QList<std::string> > getExcludePorts() const;

    /*!
     *  Set a new list of excluded ports.
     *
     *      @param [in] newExcludePorts     A new list of excluded ports.
     */
    void setExcludePorts(QSharedPointer<QList<std::string> > newExcludePorts);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the referenced component.
    std::string componentRef_;

    //! A list of excluded ports
    QSharedPointer<QList<std::string > > excludePorts_ =
        QSharedPointer<QList<std::string> >(new QList<std::string>());

};
#endif // ACTIVEINTERFACE_H
