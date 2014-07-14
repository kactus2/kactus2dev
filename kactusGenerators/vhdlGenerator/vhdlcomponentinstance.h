/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlcomponentinstance.h
 *		Project: Kactus 2
 */

#ifndef VHDLCOMPONENTINSTANCE_H
#define VHDLCOMPONENTINSTANCE_H

#include "vhdlportmap.h"

#include <kactusGenerators/HDLGenerator/vhdlconnectionendpoint.h>
#include <kactusGenerators/HDLGenerator/HDLComponentInstance.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/component.h>

#include <library/LibraryManager/vlnv.h>

#include <QObject>
#include <QString>
#include <QMap>
#include <QTextStream>

class VhdlComponentDeclaration;
class BusInterface;
class LibraryInterface;

/*! \brief Represents one vhdl component instantiation.
 *
 */
class VhdlComponentInstance : public HDLComponentInstance {
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
	VhdlComponentInstance(QObject* parent,
        LibraryInterface* handler,
		QSharedPointer<Component> component,
		QString const& instanceName,
		QString const& viewName = QString(),
		QString const& description = QString());
	
	//! \brief The destructor
	virtual ~VhdlComponentInstance();

	/*! \brief Write the instance to the text stream.
	 *
	 * \param stream The text stream to write the instance into.
	 *
	*/
	void write(QTextStream& stream) const;

	/*! \brief Add a new port map for the instance.
	 *
	 * \param endpoint The end point that specifies the port and signal bounds.
	 * \param signalName The name of the signal/top port to map the port to.
	 *
	*/
	void mapToPort(const VhdlConnectionEndPoint& endpoint, QString const& signalName);

	/*! \brief Add a new port map for the instance.
	 *
	 * \param portName The name of the port on this instance.
	 * \param portLeft The left bound of this instance's port.
	 * \param portRight The right bound of this instance's port.
	 * \param portType The type of the port.
	 * \param signalName The name of the signal/top port to connect the instance to.
	 * \param signalLeft The left bound of the signal/top port.
	 * \param signalRight The right bound of the signal/top port.
	 * \param signalType The type of the signal.
	 *
	*/
	void mapToSignal(QString const& portName, 
		int portLeft, 
		int portRight,
		QString const& portType,
		QString const& signalName,
		int signalLeft,
		int signalRight,
		QString const& signalType);

	/*! \brief Check if the specified port is connected within this instance.
	 *
	 * \param portName The name of the port to check to connection for.
	 *
	 * \return bool True if connection is found.
	*/
	bool hasConnection(QString const& portName);

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
	QSharedPointer<BusInterface> interface(QString const& interfaceName) const;

	/*! \brief Get the vlnv of the instance's IP-Xact model.
	 *
	 *
	 * \return VLNV Identifies the IP-Xact model of the instance's type.
	*/
	VLNV vlnv() const;

	/*! \brief Check if the instance has a specified port.
	 *
	 * \param portName The name of the port to check.
	 *
	 * \return bool True if the port is found.
	*/
	bool hasPort(QString const& portName) const;

	/*! \brief Check if the type of the port is scalar.
	 *
	 * \param portName Specifies the port to check.
	 *
	 * \return bool True if port is scalar.
	*/
	bool isScalarPort(QString const& portName) const;

	/*! \brief Get the direction of the specified port on this instance.
	 *
	 * \param portName The name of the port that's direction is requested.
	 *
	 * \return General::Direction Specifies the direction of the port.
	*/
	General::Direction portDirection(QString const& portName) const;

	/*! \brief Get the physical left bound of the port.
	 *
	 * \param portName The name of the port that's bound is wanted.
	 *
	 * \return int The left bound.
	*/
	int getPortPhysLeftBound(QString const& portName) const;

	/*! \brief Get the physical right bound of the port.
	 *
	 * \param portName The name of the port that's bound is wanted.
	 *
	 * \return int The right bound.
	*/
	int getPortPhysRightBound(QString const& portName) const;

signals:

	//! \brief Send a notification to user.
	void noticeMessage(QString const& noticeMessage);

	//! \brief Send an error message to user.
	void errorMessage(QString const& errorMessage);

private:
	//! \brief No copying
	VhdlComponentInstance(const VhdlComponentInstance& other);

	//! \brief No assignment
	VhdlComponentInstance& operator=(const VhdlComponentInstance& other);

    void writePortMaps(QTextStream& stream) const;

    void writeGenericAssignments(QTextStream& stream) const;

    void addMapping(const VhdlPortMap& instancePort, const VhdlPortMap& signalMapping );

    virtual QString portType(QString const& portName) const;


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
	QMap<QString, QString> modelParameterValues_;

	/*! \brief Contains the port mappings for this instance.
	 * 
	 * Key: The port name and bounds of this instance.
	 * Value: The name of the signal/top port and it's bounds the port is connected to.
	 */
	QMap<VhdlPortMap, VhdlPortMap> portMap_;
};

#endif // VHDLCOMPONENTINSTANCE_H
