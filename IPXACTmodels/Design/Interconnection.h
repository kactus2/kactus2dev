//-----------------------------------------------------------------------------
// File: Interconnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.08.2015
//
// Description:
// Describes the ipxact:interconnection element in an IP-XACT document.
//-----------------------------------------------------------------------------

#ifndef INTERCONNECTION_H
#define INTERCONNECTION_H

#include "ActiveInterface.h"
#include "HierInterface.h"

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
// Describes the ipxact:interconnection element. Specifies a connection between the bus interfaces of components.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Interconnection : public NameGroup, public Extendable
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] name            The name of the interconnection.
     *      @param [in] startInterface  The start interface.
     *      @param [in] displayName     The display name.
     *      @param [in] description     The description.
     */
    Interconnection(std::string const& name, QSharedPointer<ActiveInterface> startInterface,
        std::string const& displayName = std::string(), std::string const& description = std::string());

    /*!
     *  The constructor.
     */
    Interconnection();

    /*!
     *  Copy constructor.
     */
    Interconnection(const Interconnection& other);

    /*!
     *  The destructor.
     */
    ~Interconnection() override = default;

    /*!
     *  Assignment operator.
     */
    Interconnection& operator=(const Interconnection& other);

    /*!
     *  Checks if the interconnection is treated as an off-page connection.
     *
     *      @return True if off-page connection, otherwise false.
     */
    bool isOffPage() const;

    /*!
     *  Set/unset the connection to off-page connection.
     *
     *      @param [in] offPage   True if set to off-page, otherwise false.
     */
    void setOffPage(bool offpage);

    /*!
     *  Get the start interface.
     *
     *      @return Pointer to the start interface.
     */
    QSharedPointer<ActiveInterface> getStartInterface() const;

    /*!
     *  Set the start interface.
     *
     *      @param [in] newStartInterface   The new start interface.
     */
    void setStartInterface(QSharedPointer<ActiveInterface> newStartInterface);

    /*!
     *  Get the active interfaces.
     *
     *      @return A pointer to a list containing the active interfaces.
     */
    QSharedPointer<QList<QSharedPointer<ActiveInterface> > > getActiveInterfaces() const;

    /*!
     *  Get the hier interfaces.
     *
     *      @return A pointer to a list containing the hier interfaces.
     */
    QSharedPointer<QList<QSharedPointer<HierInterface> > > getHierInterfaces() const;

    /*!
     *  Get the presence.
     *
     *      @return The presence.
     */
    std::string getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new presence.
     */
    void setIsPresent(std::string const& newIsPresent);

    /*!
     *  Check if an interface references a given component and a bus interface.
     *
     *      @param [in] instanceName        Name of the component.
     *      @param [in] busInterfaceName    Name of the bus interface.
     *
     *      @return True, if an interface references the given component and bus interface, otherwise false.
     */
    bool hasInterfaceReferencingComponent(std::string const& instanceName, 
        std::string const& busInterfaceName) const;

    /*!
     *  Change the component reference of contained interfaces.
     *
     *      @param [in] oldName     The old component reference.
     *      @param [in] newName     The new component reference.
     */
    void changeInterfaceComponentReferences(std::string const& oldComponent, std::string const& newComponent);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The first interface.
    QSharedPointer<ActiveInterface> startInterface_;

    //! A list of active interfaces.
    QSharedPointer<QList<QSharedPointer<ActiveInterface> > > activeInterfaces_ =
        QSharedPointer<QList<QSharedPointer<ActiveInterface> > >(new QList<QSharedPointer<ActiveInterface> >());

    //! A list of hier interfaces.
    QSharedPointer<QList<QSharedPointer<HierInterface> > > hierInterfaces_ =
        QSharedPointer<QList<QSharedPointer<HierInterface> > >(new QList<QSharedPointer<HierInterface> >());

    //! The presence.
    std::string isPresent_;

};
#endif // INTERCONNECTION_H
