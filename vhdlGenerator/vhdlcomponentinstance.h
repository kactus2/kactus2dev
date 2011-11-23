/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlcomponentinstance.h
 *		Project: Kactus 2
 */

#ifndef VHDLCOMPONENTINSTANCE_H
#define VHDLCOMPONENTINSTANCE_H

#include "vhdlportmap.h"
#include "vhdlconnectionendpoint.h"

#include <models/generaldeclarations.h>
#include <models/component.h>

#include <LibraryManager/vlnv.h>

#include <QObject>
#include <QString>
#include <QMap>
#include <QTextStream>

class VhdlGenerator2;
class VhdlComponentDeclaration;
class BusInterface;

/*! \brief Represents one vhdl component instantiation.
 *
 */
class VhdlComponentInstance : public QObject {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this component instance.
	 * \param compDeclaration Pointer to the component declaration of this instance.
	 * \param instanceName The name of the instance.
	 * \param viewName The active view used for this instance.
	 * \param description The description for the instance.
	 *
	*/
	VhdlComponentInstance(VhdlGenerator2* parent,
		VhdlComponentDeclaration* compDeclaration,
		const QString& instanceName,
		const QString& viewName = QString(),
		const QString& description = QString());
	
	//! \brief The destructor
	virtual ~VhdlComponentInstance();

	/*! \brief Write the instance to the text stream.
	 *
	 * \param stream The text stream to write the instance into.
	 *
	*/
	void write(QTextStream& stream) const;

	/*! \brief Get the name of the component instance.
	 *
	 * \return QString contains the name of the instance.
	*/
	QString name() const;

	/*! \brief Get the type name of the instance.
	 *
	 * \return QString contains the type name.
	*/
	QString typeName() const;

	/*! \brief Add a new port map for the instance.
	 *
	 * \param endPoint The end point that specifies the port and signal bounds.
	 * \param signalName The name of the signal/top port to map the port to.
	 *
	*/
	void addPortMap(const VhdlConnectionEndPoint& endPoint, const QString& signalName);

	/*! \brief Add a new port map for the instance.
	 *
	 * \param portName The name of the port on this instance.
	 * \param portLeft The left bound of this instance's port.
	 * \param portRight The right bound of this instance's port.
	 * \param signalName The name of the signal/top port to connect the instance to.
	 * \param signalLeft The left bound of the signal/top port.
	 * \param signalRight The right bound of the signal/top port.
	 *
	*/
	void addPortMap(const QString& portName, int portLeft, int portRight,
		const QString& signalName, int signalLeft, int signalRight);

	/*! \brief Add a new generic map to the instance.
	 *
	 * \param genericName The name of the generic on this instance.
	 * \param genericValue The value to map the generic to.
	 *
	*/
	void addGenericMap(const QString& genericName, const QString& genericValue);

	/*! \brief Check if the specified port is connected within this instance.
	 *
	 * \param portName The name of the port to check to connection for.
	 *
	 * \return bool True if connection is found.
	*/
	bool hasConnection(const QString& portName);

	/*! \brief Use default values for ports that don't have any connection.
	 * 
	 * If at least one bit of the port is connected then the default value is
	 * not used.
	 *
	*/
	void useDefaultsForOtherPorts();

	/*! \brief Get the bus interface with given name.
	 *
	 * \param interfaceName The name of the bus interface on the IP-Xact component model.
	 *
	 * \return BusInterface* Pointer to the bus interface.
	*/
	BusInterface* interface(const QString& interfaceName) const;

	/*! \brief Get the vlnv of the instance's IP-Xact model.
	 *
	 *
	 * \return VLNV Identifies the IP-Xact model of the instance's type.
	*/
	VLNV vlnv() const;

	/*! \brief Get pointer to the IP-Xact model of the component type.
	 *
	 * \return QSharedPointer<Component> Pointer to the IP-Xact model.
	*/
	QSharedPointer<Component> componentModel() const;

	/*! \brief Get the type of the specified port.
	 *
	 * \param portName The name of the port on the component.
	 *
	 * \return QString The type of the port.
	*/
	QString portType(const QString& portName) const;

	/*! \brief Check if the instance has a specified port.
	 *
	 * \param portName The name of the port to check.
	 *
	 * \return bool True if the port is found.
	*/
	bool hasPort(const QString& portName) const;

	/*! \brief Check if the type of the port is scalar.
	 *
	 * \param portName Specifies the port to check.
	 *
	 * \return bool True if port is scalar.
	*/
	bool isScalarPort(const QString& portName) const;

	/*! \brief Get the direction of the specified port on this instance.
	 *
	 * \param portName The name of the port that's direction is requested.
	 *
	 * \return General::Direction Specifies the direction of the port.
	*/
	General::Direction portDirection(const QString& portName) const;

signals:

	//! \brief Send a notification to user.
	void noticeMessage(const QString& noticeMessage);

	//! \brief Send an error message to user.
	void errorMessage(const QString& errorMessage);

private:
	//! \brief No copying
	VhdlComponentInstance(const VhdlComponentInstance& other);

	//! \brief No assignment
	VhdlComponentInstance& operator=(const VhdlComponentInstance& other);

	void addMapping(const VhdlPortMap& instancePort, const VhdlPortMap& signalMapping );

	VhdlComponentDeclaration* compDeclaration_;

	//! \brief The name of the component instance.
	QString instanceName_;

	//! \brief The name of the type that is is an instantiation of.
	QString typeName_;

	//! \brief The name of the architecture used in this instance.
	QString architecture_;

	//! \brief The description of the instance.
	QString description_;

	/*! \brief Contains the default values for the ports of this instance.
	 * 
	 * Key: The name of the port.
	 * Value: The default value for the port.
	 */
	QMap<QString, QString> defaultPortConnections_;

	/*! \brief Contains the generic mappings for this instance.
	 * 
	 * Key: The name of the generic.
	 * Value: The value mapped for the generic.
	 */
	QMap<QString, QString> genericMap_;

	/*! \brief Contains the port mappings for this instance.
	 * 
	 * Key: The port name and bounds of this instance.
	 * Value: The name of the signal/top port and it's bounds the port is connected to.
	 */
	QMap<VhdlPortMap, VhdlPortMap> portMap_;
};

#endif // VHDLCOMPONENTINSTANCE_H
