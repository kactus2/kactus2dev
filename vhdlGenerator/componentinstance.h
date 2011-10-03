/* 
 *
 *  Created on: 2.11.2010
 *      Author: Antti Kamppi
 */

#ifndef COMPONENTINSTANCE_H_
#define COMPONENTINSTANCE_H_

#include <models/component.h>
#include "connection.h"
#include "vhdlgenerator.h"

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>
#include <QTextStream>
#include <QSharedPointer>

typedef QSharedPointer<Connection> ConnectionPointer;
typedef Connection Signal;

#if QT_VERSION < 0x070000 
// Qt 4.6 needs this to build:
inline bool operator<(
   const ConnectionPointer& p1,
   const ConnectionPointer& p2) {
  return p1.data() < p2.data();
}
#endif

class BusInterface;
class ComponentType;

class ComponentInstance: public QObject {

	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param instanceName The instance name defined in design
	 * \param typeName the name of the component type containing the library.
	 * <library>.<component name>
	 */
	ComponentInstance(const QString& instanceName, 
		const QString& typeName,
			QSharedPointer<Component> component,
			ComponentType* compDeclaration,
			QObject* parent = 0);

	//! \brief The destructor.
	virtual ~ComponentInstance();

	void createPortMap(const QString& signalName, int signalLeft, int signalRight,
		const QString& portName, int portLeft, int portRight);

	/*! \brief Add a new port map for the component instance
	 *
	 * The component instance takes ownership of the given signal and port.
	 *
	 * \param port Pointer to the connection definition of the port
	 * \param signal Pointer to the connection definition of the signal
	 */
	void addPortMap(Connection* port, Connection* signal);

	/*! \brief Set a generic map that defines generic values.
	 *
	 * \param instanceGeneric Name of the generic on the component.
	 * \param value The value for the generic or the name of a top-level generic
	 */
	void setGenericMap(const QString instanceGeneric, const QString value);

	/*! \brief Writes the component instance into given stream.
	 *
	 * Before calling this function all generic and port maps must have been
	 * defined. If a port is left unconnected then it must be mapped to "open".
	 * If generic is left unconnected then the mapping must be empty.
	 */
	void generateInstanceVhdl(QTextStream& stream);

	/*! \brief Get the name of the signal.
	 *
	 * \return QString containing the name.
	 */
	QString getName() const;

	/*! \brief Get pointer to the component model.
	 *
	 * \return A pointer to the component model
	 */
	QSharedPointer<Component> getComponent() const;

	/*! \brief Get pointer to the component declaration of this instance
	 *
	 * \return pointer to the component declaration.
	*/
	ComponentType* getCompDeclaration();

	/*! \brief Create a port declaration for the component type of this instance.
	 *
	 * \param endPoint The endpoint that identifies the port.
	 *
	*/
	void createPortDeclaration(const Connection::EndPoint& endPoint);

signals:

	//! \brief Send a error message to user.
	void errorMessage(const QString& message);

	//! \brief Send a notification to user.
	void noticeMessage(const QString& message);

private:

	//! No copying
	ComponentInstance(const ComponentInstance& other);

	//! No assignment
	ComponentInstance& operator=(const ComponentInstance& other);

	//! \brief Name of the component instance
	QString instanceName_;

	/*! \brief Name of the component type. This must contain the library name.
	 *
	 * This contains the component's name
	 */
	QString componentType_;

	//! \brief Pointer to the component declaration
	ComponentType* compDeclaration_;

	/*! \brief Contains the connections defined for the component instance.
	 *
	 * Key = name of the generic in this instance.
	 * Value = value of the generic or name of a top-level generic.
	 *
	 */
	QMap<QString, QString> genericMap_;

	/*! \brief Contains the port mappings for the component instance.
	 *
	 * Key = Connection that defines which port and which bits are connected
	 * from this component instance
	 *
	 * Value = Connection that defines the signal and bits to which the key
	 * definition is connected to.
	 */
	QMap<QSharedPointer<Connection>, QSharedPointer<Connection> > portMap_;

	/*! \brief A pointer to the IP-Xact component model to find bus interfaces.
	 * The component instance does not own the component model, only has pointer
	 *
	 * ComponentInstance does not own the component.
	 */
	QSharedPointer<Component> component_;
};

#endif /* COMPONENTINSTANCE_H_ */
