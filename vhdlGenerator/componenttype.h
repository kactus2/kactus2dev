/* 
 *
 *  Created on: 2.11.2010
 *      Author: Antti Kamppi
 */

#ifndef COMPONENTTYPE_H_
#define COMPONENTTYPE_H_

#include <models/generaldeclarations.h>

#include <QObject>
#include <QString>
#include <QTextStream>
#include <QMap>
#include <QSharedPointer>
#include <QStringList>

class Connection;
class Component;

class ComponentType: public QObject {

public:

	/*! \brief Contains one generic-value related information.
	 *
	 * Contains 2 strings, name and type.
	 */
	struct VhdlGeneric {

		//! \brief The name of the generic
		QString name_;

		//! \brief The type of the generic (i.e. integer).
		QString type_;

		//! \brief The optional default value of the generic.
		QString defaultValue_;

		//! \brief The description for the generic.
		QString description_;

		/*! \brief The struct constructor
		 *
		 * \param name The name of the generic
		 * \param type The type of the generic
		 * \param defaultValue The default value for the generic
		 * \param description The description for the generic
		 */
		VhdlGeneric(const QString name, const QString type,
				const QString& defaultValue = QString(),
				const QString& description = QString());
	};

	/*! \brief The constructor
	 *
	 * \param parent A pointer to this widget's parent
	 *
	 * Exception guarantee: basic
	 * \exception Vhdl_error Occurs if the component pointer is null.
	 */
	ComponentType(QSharedPointer<Component> component, QObject* parent = 0);

	//! \brief The destructor
	virtual ~ComponentType();

	/*! \brief Write the component declaration into stream.
	 *
	 * \param stream A reference to the stream to write declaration into.
	 *
	 * Exception guarantee: basic
	 * \exception Vhdl_error Occurs if an invalid port type has been defined
	 * for a component port.
	 */
	void generateComponentVhdl(QTextStream& vhdlStream);

	/*! \brief Get the component type name
	 *
	 * \return QString containing the type name.
	 */
	QString getType() const;

	/*! \brief Get the component instance type that contains the library
	 *
	 * The instance type is form <library_>.<typeName>
	 *
	 * \return QString containing the instance type.
	 */
	QString getInstanceType() const;

	/*! \brief Does the component type have a generic of the specified name.
	 *
	 * \return True if generic is found. False if not found.
	 */
	bool hasGeneric(const QString& name) const;

    /*! \brief Returns generic type of the given generic name. Generic must
     *         exits.
     *
     *  \param Generic name. Must exist
     *  \return Type of the generic in lower case
     */
    QString genericType(const QString& name) const;

	/*! \brief Parses the generics and ports from given component.
	 *
	 * This function is used to search all generics and ports in a component
	 * and add them to appropriate data structures.
	 */
	static void parseInterface(QSharedPointer<Component> component,
			QMap<QString, QSharedPointer<VhdlGeneric> >& generics);

	/*! \brief Get list of ports for the component declaration
	 *
	 * \return QStringList containing the port names.
	*/
	QStringList getPortNames() const;

	/*! \brief Create a new port for the component declaration.
	 *
	 * \param portName Name of the port.
	 * \param direction Defines the direction of the port.
	 * \param left Left bound of the port.
	 * \param right Right bound of the port.
	 * \param description Contains the description for the port.
	 *
	*/
	void createPort(const QString& portName, General::Direction direction, 
		int left, int right, const QString& description = QString());

private:

	//! No copying
	ComponentType(const ComponentType& other);

	//! No assignment
	ComponentType& operator=(const ComponentType& other);

	//! \brief The type name of the component.
	QString typeName_;

	/*! \brief Contains the generic declarations for this component type.
	 *
	 * Key = Name of the generic
	 * Value = A pointer to the generic
	 *
	 * ComponentType owns the generics.
	 */
	QMap<QString, QSharedPointer<VhdlGeneric> > generics_;

	/*! \brief Contains the port declarations for this component type.
	 *
	 * Key = Name of the port
	 * Value = A pointer to the port definition
	 *
	 * ComponentType owns the ports
	 */
	QMap<QString, QSharedPointer<Connection> > ports_;

    /* \brief Add quotation marks to string generics if needed
     *
     * \param type Generic type
     * \param value Original generic value
     * \return Quoted string generic
     */
    QString quoteGenericValue(const QString& type, const QString& value) const;

	//! \brief The description for the component declaration.
	QString description_;

};

#endif /* COMPONENTTYPE_H_ */
