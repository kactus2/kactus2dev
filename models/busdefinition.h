/* 
 *
 *  Created on: 15.7.2010
 *      Author: Antti Kamppi
 */

#ifndef BUSDEFINITION_H_
#define BUSDEFINITION_H_

#include "librarycomponent.h"

#include <common/KactusAttribute.h>

#include <LibraryManager/vlnv.h>

#include <common/Global.h>
 
#include <QDomDocument>
#include <QSharedPointer>
#include <QStringList>
#include <QList>
#include <QFile>
#include <QMap>

/*! \brief Equals the spirit:busDefinition element in IP-Xact specification
 *
 * BusDefinition is one of the top-level elements in IP-Xact and is used to
 * describe high-level aspects of a bus.
 */
class KACTUS2_API BusDefinition: public LibraryComponent {

public:

	/*! \brief The constructor
	 *
	 * \param doc A reference to the QDomDocument to parse the information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	BusDefinition(QDomDocument &doc);

	/*! \brief The default constructor
	 *
	 * This constructor is used to create an empty busDefinition.
	 */
	BusDefinition();

	//! \brief Copy constructor
	BusDefinition(BusDefinition const& other);

	//! \brief Assignment operator
	BusDefinition &operator=(BusDefinition const& other);

	//! \brief The destructor
	virtual ~BusDefinition();

	/*! \brief Clone this bus definition and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit LibraryComponent.
	 *
	 * \return QSharedPointer<LibraryComponent> Pointer to the cloned bus definition.
	*/
	virtual QSharedPointer<LibraryComponent> clone() const;

	/*! \brief Write the document contents into file.
	*
	* Prints all information stored in this document into specified file
	* generating a valid IP-Xact document.
	*
	* \param file A reference to QFile instance representing the file to write
	* the data into.
	*/
	virtual void write(QFile& file);

	/*! \brief Check the validity of the bus definition.
	 * 
	 * This function should be used to check if the bus definition is in valid state
	 * before writing the xml to the disk.
	 * 
	 * \param errorList The error messages of the detected errors are appended to
	 * this list.
	 *
	 * \return bool True if the bus definition was valid.
	*/
	virtual bool isValid(QStringList& errorList) const;

	/*! \brief Check if the bus definition is in a valid state.
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

	/*! \brief Get the directConnection setting
	 *
	 * \return boolean value of the directConnection
	 */
	bool getDirectConnection() const;

	/*! \brief Get the vlnv tag of the bus definition that this definition
	 * extends
	 *
	 * If this definition does not extend any other definition then returns 0
	 * \return A pointer to the extended vlnv. 0 if no vlnv is extended.
	 */
	VLNV getExtends() const;

	/*! \brief Get the isAdressable element
	 *
	 * \return boolean value of the isAddressable element
	 */
	bool getIsAddressable() const;

	/*! \brief Get the maximum number of masters on this bus
	 *
	 * \return unsigned int containing maximum number of masters
	 */
	int getMaxMasters() const;

	/*! \brief Get the maximum number of slaves on this bus
	 *
	 * \return unsigned int containing maximum number of slaves
	 */
	int getMaxSlaves() const;

	/*! \brief Get the systemGroupNames
	 *
	 * \return QList containing names of systemGroups
	 */
	const QStringList& getSystemGroupNames();

	/*! \brief Get the attributes of the busDefinition
	 *
	 * \return QMap containing the attributes.
	 */
	const QMap<QString, QString>& getAttributes();

	/*! \brief Set the attributes for the busDefinition.
	 *
	 * Calling this function will erase any old attributes.
	 *
	 * \param attributes QMap containing new attributes
	 */
	void setAttributes(const QMap<QString, QString>& attributes);

	/*! \brief Set the directConnection setting
	 *
	 * \param directConnection The wanted setting
	 */
	void setDirectConnection(bool directConnection);

	/*! \brief Set the extends vlnv tag
	 *
	 * Calling this function deletes the old extends element.
	 *
	 * \param extends The vlnv tag that of bus definition that this definition
	 * extends
	 */
	void setExtends(const VLNV& extends);

	/*! \brief Set the isAddressable setting
	 *
	 * \param isAddressable The wanted setting
	 */
	void setIsAddressable(bool isAddressable);

	/*! \brief Set the maximum number of masters on this bus
	 *
	 * \param maxMasters The maximum number of masters
	 */
	void setMaxMasters(int maxMasters);

	/*! \brief Set the maximum number of slaves on this bus
	 *
	 * \param The maximum number of slaves
	 */
	void setMaxSlaves(int maxSlaves);

	/*! \brief Set the systemGroupNames
	 *
	 * \systemGroupNames The QList containing the names of the systemGroups
	 */
	void setSystemGroupNames(const QStringList& systemGroupNames);

	/*! \brief Get the file dependecies of this busDefinition
	 *
	 * This function returns always empty QStringList because busDefinition
	 * only has VLNV dependencies. This function exists because the base class
	 * function is pure virtual function.
	 *
	 * \return QStringList empty list.
	 */
	virtual const QStringList getDependentFiles() const;

	/*! \brief Get the VLNVs that this busDefinition needs.
	 *
	 * This function returns list of pointers to VLNVs that are needed by this
	 * busDefinition.
	 *
	 * \return QList containing the VLNVs.
	 */
	virtual const QList<VLNV> getDependentVLNVs() const;

    /*!
     *  Sets the Kactus-specific bus definition type.
     *
     *      @param [in] type The bus definition type to set.
     */
    void setType(KactusAttribute::BusDefType type);

    /*!
     *  Returns the Kactus-specific bus definition type.
     */
    KactusAttribute::BusDefType getType() const;


private:

	/*! \brief Direct connection specifies what connections are allowed.
	 * spirit: directConnection
	 * MANDATORY
	 */
	bool directConnection_;

	/*! \brief Specifies if the bus has addressing information.
	 * spirit:isAddressable
	 * MANDATORY
	 */
	bool isAddressable_;

	/*! \brief Max number of masters allowed on this bus.
	 * spirit:maxMasters
	 * OPTIONAL
	 * value -1 means unbounded
	 */
	int maxMasters_;

	/*! \brief Max number of slaves allowed on this bus.
	 * spirit:maxSlaves
	 * OPTIONAL
	 * value -1 means unbounded
	 */
	int maxSlaves_;

	/*! \brief List of systemGroupName elements.
	 * spirit:systemGroupNames
	 * OPTIONAL
	 */
	QStringList systemGroupNames_;

	/*! \brief specifies if this definition is an extension from another bus.
	 * spirit:extends
	 * OPTIONAL
	 */
	VLNV extends_;

	//! \brief Contains the attributes for the spirit:busDefinition element.
	//! OPTIONAL
	QMap<QString, QString> attributes_;

};

#endif /* BUSDEFINITION_H_ */
