//-----------------------------------------------------------------------------
// File: HDLComponentInstance.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Base class for HDL component instances.
//-----------------------------------------------------------------------------

#ifndef HDLCOMPONENTINSTANCE_H
#define HDLCOMPONENTINSTANCE_H

#include "HDLObject.h"
#include "vhdlconnectionendpoint.h"

#include <IPXACTmodels/component.h>

#include <QObject>
#include <QString>
#include <QMap>
#include <QTextStream>

class HDLComponentDeclaration;

//-----------------------------------------------------------------------------
//! A base class for all HDL component instance implementations.
//-----------------------------------------------------------------------------
class HDLComponentInstance : public HDLObject
{
    Q_OBJECT

public:
    
	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent          The parent object.
	 *      @param [in] component       The component the instance represents.
	 *      @param [in] instanceName    The name of the instance.
	 *      @param [in] viewName        The name of the active view of the instance.
	 *      @param [in] description     The description of the instance.	 
	 */
	HDLComponentInstance(QObject* parent, QSharedPointer<Component> component,
		const QString& instanceName, QString const& viewName,
		const QString& description = QString());
	
	//! The destructor
	virtual ~HDLComponentInstance();

	/*! Add a new port map for the instance.
	 *
	 *      @param [in] endpoint    The end point that specifies the port and signal bounds.
	 *      @param [in] signalName  The name of the signal/top port to map the port to.
	 *
	 */
	virtual void mapToPort(VhdlConnectionEndPoint const& endpoint, const QString& signalName) = 0;

	/*! Add a new port map for the instance.
	 *
	 *      @param [in] portName    The name of the port on this instance.
	 *      @param [in] portLeft    The left bound of this instance's port.
	 *      @param [in] portRight   The right bound of this instance's port.
	 *      @param [in] portType    The type of the port.
	 *      @param [in] signalName  The name of the signal/top port to connect the instance to.
	 *      @param [in] signalLeft  The left bound of the signal/top port.
	 *      @param [in] signalRight The right bound of the signal/top port.
	 *      @param [in] signalType  The type of the signal.
	 */
	virtual void mapToSignal(const QString& portName, int portLeft, int portRight, const QString& portType,	
        const QString& signalName, int signalLeft, int signalRight, const QString& signalType) = 0;

	/*! Add a new model parameter assignment to the instance.
	 *
	 *      @param [in] modelParameterName     The name of the model parameter on this instance.
	 *      @param [in] modelParameterValue    The value to map the model parameter to.	 
	 */
    void assignModelParameter(const QString& modelParameterName, const QString& modelParameterValue);

	/*! Check if the specified port is connected within this instance.
	 *
	 *      @param [in] portName The name of the port to check to connection for.
	 *
	 *      @return bool True if connection is found, otherwise false.
	*/
	virtual bool hasConnection(const QString& portName) = 0;

	/*! Use default values for ports that don't have any connection.
	 * 
	 * If at least one bit of the port is connected then the default value is not used.
	 *
	 */
	virtual void useDefaultsForOtherPorts() = 0;

	/*! Get the bus interface with given name.
	 *
	 *      @param [in] interfaceName   The name of the bus interface on the IP-Xact component model.
	 *
	 *      @return The bus interface.
	 */
	QSharedPointer<BusInterface> interface(const QString& interfaceName) const;

	/*! Get the vlnv of the instance's IP-Xact model.	 
	 *
	 *      @return VLNV Identifies the IP-Xact model of the instance's type.
	*/
	VLNV vlnv() const;

	/*! Get the IP-Xact model of the component type.
	 *
	 *      @return The IP-Xact model.
	*/
	QSharedPointer<Component> componentModel() const;

	/*! Get the type of the specified port.
	 *
	 *      @param [in] portName    The name of the port on the component.
	 *
	 *      @return The type of the port.
	*/
	virtual QString portType(const QString& portName) const = 0;

	/*! Check if the instance has a specified port.
	 *
	 *      @param [in] portName The name of the port to check.
	 *
	 *      @return True if the port is found, otherwise false.
	*/
	bool hasPort(const QString& portName) const;

	/*! Check if the type of the port is scalar.
	 *
	 *      @param [in] portName Specifies the port to check.
	 *
	 *      @return True if port is scalar.
	*/
	virtual bool isScalarPort(const QString& portName) const = 0;

	/*! Get the direction of the specified port on this instance.
	 *
	 *      @param [in] portName The name of the port that's direction is requested.
	 *
	 *      @return The direction of the port.
	*/
	General::Direction portDirection(const QString& portName) const;

	/*! Get the physical left bound of the port.
	 *
	 *      @param [in] portName The name of the port that's bound is wanted.
	 *
	 *      @return The left bound.
	*/
	int getPortPhysLeftBound(const QString& portName) const;

	/*! Get the physical right bound of the port.
	 *
	 *      @param [in] portName The name of the port that's bound is wanted.
	 *
	 *      @return The right bound.
	*/
	int getPortPhysRightBound(const QString& portName) const;

signals:
    
    //! Emitted to send a notification to user.
    void noticeMessage(QString const& noticeMessage);

    //! Emitted to send an error message to user.
    void errorMessage(QString const& errorMessage);

protected:

    /*!
     *  Sets the instance architecture.
     *
     *      @param [in] architecture   The architecture to set.     
     */
    void setArchitecture(QString const& architecture);

    /*!
     *  Gets the instance architecture.
     *
     *      @return The instance architecture.
     */
    QString getArchitecture() const;
    
private:
	//! No copying
	HDLComponentInstance(const HDLComponentInstance& other);

	//! No assignment
	HDLComponentInstance& operator=(const HDLComponentInstance& other);

    //! The component represented by the instance.
    QSharedPointer<Component> component_;

    //! The name of the architecture used in this instance.
    QString architecture_;

	/*! Contains the default values for the ports of this instance.
	 * 
	 * Key: The name of the port.
	 * Value: The default value for the port.
	 */
	QMap<QString, QString> defaultPortConnections_;

	/*! Contains the generic mappings for this instance.
	 * 
	 * Key: The name of the generic.
	 * Value: The value mapped for the generic.
	 */
	QMap<QString, QString> modelParameterValues_;

};

#endif // HDLCOMPONENTINSTANCE_H
