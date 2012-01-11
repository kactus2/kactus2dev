/* 
 *
 *  Created on: 12.8.2010
 *      Author: Antti Kamppi
 */

#ifndef ABSTRACTIONDEFINITION_H_
#define ABSTRACTIONDEFINITION_H_

#include "librarycomponent.h"

#include "generaldeclarations.h"
  
#include <QList>
#include <QDomDocument>
#include <QSharedPointer>
#include <QStringList>
#include <QFile>
#include <QMap>
#include <QStringList>

class VLNV;
class PortAbstraction;

/*! \brief Equals the spirit:abstractionDefinition element in IP-Xact
 * specification.
 *
 * AbstractionDefinitions describes the low-level aspects of a bus or
 * interconnect.
 */
class AbstractionDefinition: public LibraryComponent {

public:

	/*! \brief The constructor
	 *
	 * Exception guarantee : basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or some member class.
	 *
	 * \param doc A reference to a QDomDocument to parse the information from.
	 */
	AbstractionDefinition(QDomDocument& doc);

	//! Copy constructor
	AbstractionDefinition(const AbstractionDefinition& other);

	//! Assignment operator
	AbstractionDefinition& operator=(const AbstractionDefinition& other);

	/*! \brief Default constructor
	 *
	*/
	AbstractionDefinition();

	/*! \brief The destructor
	 *
	 */
	virtual ~AbstractionDefinition();

	/*! \brief Clone this abstraction definition and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit LibraryComponent.
	 *
	 * \return QSharedPointer<LibraryComponent> Pointer to the cloned abstraction definition.
	*/
	virtual QSharedPointer<LibraryComponent> clone() const;

	/*! \brief Write the document contents into file.
	*
	* Prints all information stored in this document into specified file
	* generating a valid IP-Xact document.
	*
	* \param file A reference to QFile instance representing the file to write
	* the data into.
	*
	* \exception Write_error Occurs if the class or one of it's member classes
	* is not valid IP-Xact at the moment of writing.
	*/
	virtual void write(QFile& file);

	/*! \brief Check the validity of the abstraction definition.
	 * 
	 * This function should be used to check if the abstraction definition is in
	 * valid state before writing the xml to the disk.
	 * 
	 * \param errorList The error messages of the detected errors are appended to
	 * this list.
	 *
	 * \return bool True if the abstraction definition was valid.
	*/
	virtual bool isValid(QStringList& errorList) const;

	/*! \brief Check if the abstraction definition is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid() const;

	/*! \brief Set the vlnv
	 *
	 * \param vlnv Reference to the vlnv to set
	 *
	*/
	virtual void setVlnv(const VLNV& vlnv);

	/*! \brief Get the bus type of this abstraction definition.
	 *
	 * \return A pointer to the vlnv of the bus definition.
	 */
	VLNV getBusType() const;

	/*! \brief Get the vlnv of the abstraction definition that this definition
	 * extends
	 *
	 * If this is not an extension of any abstraction definition then null
	 * pointer is returned
	 *
	 * \return A pointer to the vlnv of the extended abstraction definition.
	 */
	VLNV getExtends() const;

	/*! \brief Get list of the ports for this abstraction definition.
	 *
	 * \return QList containing pointers to the ports
	 */
	QList<QSharedPointer<PortAbstraction> >& getPorts();

	/*! \brief Get the attributes of the abstractionDefinition
	 *
	 * \return A const reference to a QMap containing the attributes.
	 */
	const QMap<QString, QString>& getAttributes();

	/*! \brief Set the attributes for the abstractionDefinition
	 *
	 * Calling this function will reset all previous attributes
	 *
	 * \param attributes A const reference to a QMap containing new attributes.
	 */
	void setAttributes(const QMap<QString, QString>& attributes);

	/*! \brief Set the bus type for this abstraction definition.
	 *
	 * \param busType A pointer to the vlnv instance.
	 */
	virtual void setBusType(const VLNV& busType);

	/*! \brief Set the extends element
	 *
	 * \param extends A pointer to the vlnv instance of  the extended
	 * abstraction definition.
	 */
	virtual void setExtends(const VLNV& extends);

	/*! \brief Set the ports for this abstraction definition.
	 *
	 * \param ports A QList containing pointers to the ports.
	 */
	virtual void setPorts(QList<QSharedPointer<PortAbstraction> >& ports);

	/*! \brief This function has empty implementation and it returns empty list.
	 *
	 * This function exists only because this function is pure virtual function
	 * in libraryComponent.
	 *
	 * \return Empty QStringList
	 */
	virtual const QStringList getDependentFiles() const;

	/*! \brief Get the VLNVs of the IP-Xact documents needed by this document
	 *
	 * Get list of the VLNVs that this abstractionDefinition is dependent on.
	 * Return the VLNV of the busType and possibly an extension VLNV.
	 *
	 * \return QList containing pointers to the VLNVs.
	 */
	virtual const QList<VLNV> getDependentVLNVs() const;

	/*! \brief Get the logical names of the ports defined in this abstraction definition.
	 *
	 * \return QStringList containing the port names.
	*/
	QStringList getPortNames() const;

	/*! \brief Get the logical names of the ports defined for given interface mode.
	 *
	 * \param mode Specifies the mode that's ports are requested.
	 *
	 * \return QStringList containing the names of ports with "required" or "optional" status.
	*/
	QStringList getPortNames(General::InterfaceMode mode) const;

	/*! \brief Get size of specified port in specified interface mode.
	 *
	 * \param port Identifies the port.
	 * \param mode Specifies the interface mode for the port.
	 *
	 * \return int The number of bits the signal has. If unspecified then -1 is
	 * returned.
	*/
	int getPortSize(const QString& port, General::InterfaceMode mode) const;

	/*! \brief Check if specified port is required in given interface mode.
	 *
	 * \param portName Identifies the port that is checked.
	 * \param mode Specifies the mode that is checked on the port.
	 *
	 * \return bool True if specified port is required in specified mode.
	*/
	bool isRequired(const QString& portName, General::InterfaceMode mode) const;

	/*! \brief Check if specified port is optional in given interface mode.
	 *
	 * \param portName Identifies the port that is checked.
	 * \param mode Specifies the mode that is checked on the port.
	 *
	 * \return bool True if specified port is optional in specified mode.
	*/
	bool isOptional(const QString& portName, General::InterfaceMode mode) const;

	/*! \brief Check if specified port is illegal in given interface mode.
	 *
	 * \param portName Identifies the port that is checked.
	 * \param mode Specifies the mode that is checked on the port.
	 *
	 * \return bool True if specified port is illegal in specified mode.
	*/
	bool isIllegal(const QString& portName, General::InterfaceMode mode) const;

	/*! \brief Check if the given port is contained in this abs def with given interface mode.
	 *
	 * \param portName The name of the port to search.
	 * \param mode The interface mode to search for the port in.
	 *
	 * \return bool True if port is required or optional.
	*/
	bool hasPort(const QString& portName, General::InterfaceMode mode) const;

	/*! \brief Get the direction of port in specified interface mode.
	 *
	 * \param portName Identifies the logical name of the port.
	 * \param mode Specifies the interface mode for the port.
	 *
	 * \return General::Direction The direction of the port.
	*/
	General::Direction getPortDirection(const QString& portName, General::InterfaceMode mode) const;

	/*! \brief Get the default value of the port
	 *
	 * \param portName The name of the port.
	 *
	 * \return QString Contains the default value.
	*/
	int getDefaultValue(const QString& portName) const;

	/*! \brief Check if the abs def has a default value assigned for the port.
	 *
	 * \param portName The name of the port that's default value is checked.
	 *
	 * \return bool True if port has a default value assigned.
	*/
	bool hasDefaultValue(const QString& portName) const;

private:

	/*! \brief Specifies the bus definition for this abstraction definition
	 *
	 * MANDATORY spirit:busType
	 */
	VLNV busType_;

	/*! \brief A list of logical ports.
	 *
	 * MANDATORY spirit:ports
	 */
	QList<QSharedPointer<PortAbstraction> > ports_;

	/*! \brief Specifies if this definition is an extension
	 *
	 * OPTIONAL spirit:extends
	 */
	VLNV extends_;

	//! \brief Contains the attributes for the abstractionDefinition.
	QMap<QString, QString> attributes_;

};

#endif /* ABSTRACTIONDEFINITION_H_ */
