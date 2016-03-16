//-----------------------------------------------------------------------------
// File: vhdlcomponentinstance.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 26.10.2011
//
// Description:
// Represents one vhdl component instantiation.
//-----------------------------------------------------------------------------

#ifndef VHDLCOMPONENTINSTANCE_H
#define VHDLCOMPONENTINSTANCE_H

#include "vhdlobject.h"
#include "vhdlportmap.h"
#include "vhdlconnectionendpoint.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QObject>
#include <QString>
#include <QMap>
#include <QTextStream>

class VhdlComponentDeclaration;
class BusInterface;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Represents one vhdl component instantiation.
//-----------------------------------------------------------------------------
class VhdlComponentInstance : public QObject, public VhdlObject
{
    Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *  @param [in] parent              Pointer to the owner of this component instance.
	 *  @param [in] compDeclaration     Pointer to the component declaration of this instance.
	 *  @param [in] instanceName        The name of the instance.
	 *  @param [in] viewName            The active view used for this instance.
	 *  @param [in] description         The description for the instance.
	 */
	VhdlComponentInstance(QObject* parent, LibraryInterface* handler, VhdlComponentDeclaration* compDeclaration,
		const QString& instanceName, const QString& viewName = QString(), const QString& description = QString());
	
	//! The destructor.
	virtual ~VhdlComponentInstance();

	/*!
     *  Write the instance to the text stream.
	 *
	 *      @param [in] stream  The text stream to write the instance into.
	 */
	void write(QTextStream& stream) const;

	/*!
     *  Get the name of the component instance.
	 *
	 *      @return QString contains the name of the instance.
	 */
	QString name() const;

	/*!
     *  Get the type name of the instance.
	 *
	 *      @return QString contains the type name.
	 */
	QString typeName() const;

	/*!
     *  Add a new port map for the instance.
	 *
	 *      @param [in] endpoint    The end point that specifies the port and signal bounds.
	 *      @param [in] signalName  The name of the signal/top port to map the port to.
	 */
	void addPortMap(const VhdlConnectionEndPoint& endpoint, const QString& signalName);

	/*!
     *  Add a new port map for the instance.
	 *
	 *      @param [in] portName        The name of the port on this instance.
	 *      @param [in] portLeft        The left bound of this instance's port.
	 *      @param [in] portRight       The right bound of this instance's port.
	 *      @param [in] portType        The type of the port.
	 *      @param [in] signalName      The name of the signal/top port to connect the instance to.
	 *      @param [in] signalLeft      The left bound of the signal/top port.
	 *      @param [in] signalRight     The right bound of the signal/top port.
	 *      @param [in] signalType      The type of the signal.
	 */
	void addPortMap(const QString& portName, const QString& portLeft, const QString& portRight,
        const QString& portType, const QString& signalName, const QString& signalLeft, const QString& signalRight,
		const QString& signalType);

	/*!
     *  Add a new generic map to the instance.
	 *
	 *      @param [in] genericName     The name of the generic on this instance.
	 *      @param [in] genericValue    The value to map the generic to.
	 */
	void addGenericMap(const QString& genericName, const QString& genericValue);

	/*!
     *  Check if the specified port is connected within this instance.
	 *
	 *      @param [in] portName    The name of the port to check to connection for.
	 *
	 *      @return bool True if connection is found.
	 */
	bool hasConnection(const QString& portName);

	/*!
     *  Use default values for ports that don't have any connection.
	 * 
	 *  If at least one bit of the port is connected then the default value is not used.
	 */
	void useDefaultsForOtherPorts();

	/*!
     *  Get the bus interface with given name.
	 *
	 *      @param [in] interfaceName   The name of the bus interface on the IP-Xact component model.
	 *
	 *      @return BusInterface* Pointer to the bus interface.
	 */
	QSharedPointer<BusInterface> interface(const QString& interfaceName) const;

	/*!
     *  Get the vlnv of the instance's IP-Xact model.
	 *
	 *      @return VLNV Identifies the IP-Xact model of the instance's type.
	 */
	VLNV vlnv() const;

	/*!
     *  Get pointer to the IP-Xact model of the component type.
	 *
	 *      @return QSharedPointer<Component> Pointer to the IP-Xact model.
	 */
	QSharedPointer<Component> componentModel() const;

	/*!
     *  Get the type of the specified port.
	 *
	 *      @param [in] portName    The name of the port on the component.
	 *
	 *      @return QString The type of the port.
	 */
	QString portType(const QString& portName) const;

	/*!
     *  Check if the instance has a specified port.
	 *
	 *      @param [in] portName    The name of the port to check.
	 *
	 *      @return bool True if the port is found.
	 */
	bool hasPort(const QString& portName) const;

	/*!
     *  Check if the type of the port is scalar.
	 *
	 *      @param [in] portName    Specifies the port to check.
	 *
	 *      @return bool True if port is scalar.
	 */
	bool isScalarPort(const QString& portName) const;

	/*!
     *  Get the direction of the specified port on this instance.
	 *
	 *      @param [in] portName    The name of the port that's direction is requested.
	 *
	 *      @return General::Direction Specifies the direction of the port.
	 */
	DirectionTypes::Direction portDirection(const QString& portName) const;

	/*!
     *  Get the physical left bound of the port.
	 *
	 *      @param [in] portName    The name of the port that's bound is wanted.
	 *
	 *      @return int The left bound.
	 */
	QString getPortPhysLeftBound(const QString& portName) const;

	/*!
     *  Get the physical right bound of the port.
	 *
	 *      @param [in] portName    The name of the port that's bound is wanted.
	 *
	 *      return int The right bound.
	 */
	QString getPortPhysRightBound(const QString& portName) const;

signals:

	//! Send a notification to user.
	void noticeMessage(const QString& noticeMessage);

	//! Send an error message to user.
	void errorMessage(const QString& errorMessage);

private:

	//! No copying.
	VhdlComponentInstance(const VhdlComponentInstance& other);

	//! No assignment.
	VhdlComponentInstance& operator=(const VhdlComponentInstance& other);

	/*!
	 *  Add port mapping.
	 *
	 *      @param [in] instancePort    Selected VHDL port map object.
	 *      @param [in] signalMapping   The VHDL mapped signal.
	 */
	void addMapping(const VhdlPortMap& instancePort, const VhdlPortMap& signalMapping );

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component declaration.
	VhdlComponentDeclaration* compDeclaration_;

	//! The name of the component instance.
	QString instanceName_;

	//! The name of the type that is is an instantiation of.
	QString componentModuleName_;

	//! The name of the architecture used in this instance.
	QString architecture_;

	//! The description of the instance.
	QString description_;

	/*!
     *  Contains the default values for the ports of this instance.
	 * 
	 *  Key: The name of the port. Value: The default value for the port.
	 */
	QMap<QString, QString> defaultPortConnections_;

	/*!
     *  Contains the generic mappings for this instance.
	 * 
	 *  Key: The name of the generic. Value: The value mapped for the generic.
	 */
	QMap<QString, QString> genericMap_;

	/*!
     *  Contains the port mappings for this instance.
	 * 
	 *  Key: The port name and bounds of this instance.
	 *  Value: The name of the signal/top port and it's bounds the port is connected to.
	 */
	QMap<VhdlPortMap, VhdlPortMap> portMap_;
};

#endif // VHDLCOMPONENTINSTANCE_H
