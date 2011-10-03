/* 
 *
 *  Author: Antti Kamppi
 */

#ifndef VLNV_H_
#define VLNV_H_

#include <QString>
#include <QMetaType>

/*! \brief The class to hold VLNV information of a single IP-XACT document
 *
 * This class provides services to store, read and write VLNV information
 * easily in classes that handle IP-XACT data.
 */
class VLNV {

public:

	/*! \brief Enum Type is used to tell the type of IP-XACT document
	 *
	 */
	enum IPXactType {BUSDEFINITION = 0, 		// spirit:busDefinition
				COMPONENT = 1,					// spirit:component
				DESIGN = 2, 					// spirit:design
				GENERATORCHAIN = 3,				// spirit:generatorChain
				ABSTRACTOR = 4,					// spirit:abstractor
				DESIGNCONFIGURATION = 5,		// spirit:designConfiguration
				ABSTRACTIONDEFINITION = 6,		// spirit:abstractionDefinition
				INVALID = 7 };					// invalid type

	/*! \brief Constructor with parameters
	 *
	 * \param type the type of the VLNV document (design, component...)
	 * \param vendor The name of the vendor
	 * \param library The name of the library
	 * \param name The name of the block
	 * \param version The version number of the block
	 * \param hidden Is the item created a hidden or not
	 */
	VLNV(const QString &type, const QString &vendor, const QString &library,
		const QString &name, const QString &version, const bool hidden = false);

	/*! \brief Constructor that takes type parameter as enum
	 *
	 * This constructor uses enum IPXactType to define the type of the
	 * document. filePath is optional parameter
	 * \param type IPXactType type parameter defining document type
	 * \param vendor The name of the vendor
	 * \param library The name of the library
	 * \param name The name of the block
	 * \param version The version number of the block
	 * \param hidden is the item to be created marked as hidden or not
	 */
	VLNV(const IPXactType &type, const QString &vendor, const QString &library,
			const QString &name, const QString &version,
			const bool hidden = false);

	/*! \brief The default constructor
	 *
	 * Constructs an invalid VLNV instance.
	*/
	VLNV();

	/*! \brief The destructor
	 *
	 */
	~VLNV();

	/*! \brief Copy constructor
	 *
	 * \param other The VLNV to be copied
	 */
	VLNV(const VLNV &other);

	/*! \brief Get the type the VLNV belongs to in a QString
	 *
	 * \return QString that holds the XML document element
	 */
	QString getTypestr() const;

	/*! \brief Get the type of the document the VLNV belongs to
	 *
	 * \return enum IPXactType which tells the document type
	 */
	IPXactType getType() const;

	/*! \brief Get the vendor of the VLNV
	 *
	 * \return the QString holding the name of the vendor
	 */
	QString getVendor() const;

	/*! \brief Set the vendor for the vlnv
	 *
	 * \param vendor QString containing the vendor name
	 *
	*/
	void setVendor(const QString& vendor);

	/*! \brief Get the library of the VLNV
	 *
	 * \return the QString holding the name of the library
	 */
	QString getLibrary() const;

	/*! \brief Set the library for the vlnv
	 *
	 * \param library QString containing the library name
	 *
	*/
	void setLibrary(const QString& library);

	/*! \brief Get the name of the IP block in VLNV
	 *
	 * \return the QString holdin the name of the IP block in VLNV
	 */
	QString getName() const;

	/*! \brief Set the name for the VLNV
	 *
	 * \param name QString containing the new name
	 *
	*/
	void setName(const QString name);

	/*! \brief Get the version of the VLNV
	 *
	 * \return the QString holding the version of the VLNV
	 */
	QString getVersion() const;

	/*! \brief Set the version of the VLNV
	 *
	 * \param version QString containing the new version
	 *
	*/
	void setVersion(const QString version);

	/*! \brief Is the librarycomponent this VLNV represents a hidden or not
	 *
	 * \return True if the item is hidden. False if not hidden.
	 */
	bool isHidden() const;

	/*! \brief Mark this item as not hidden
	 *
	 */
	void show();

	/*! \brief Mark this item as hidden
	 *
	 */
	void hide();

	/*! \brief Set the document type for the vlnv.
	 *
	 * \param type Specifies the type to set.
	 *
	*/
	void setType(IPXactType type);

	/*! \brief Set the document type for the vlnv
	 *
	 * \param type QString specifying the type
	 *
	*/
	void setType(const QString& type);

	/*! \brief The assignment operator for the class
	 *
	 */
	VLNV &operator=(const VLNV &other);

	/*! \brief The < operator for comparison of the VLNV info
	 *
	 * \param other the instance to compare
	 * \return boolean value of the comparison
	 */
	bool operator<(const VLNV &other) const;

	/*! \brief The > operator for comparison of the VLNV info
	 *
	 * \param other the instance to compare
	 * \return the boolean value of the comparison
	 */
	bool operator>(const VLNV &other) const;

	/*! \brief The == comparison operator
	 *
	 * \param other The other VLNV instance to be compared
	 * \return boolean value of the comparison operation
	 */
	bool operator==(const VLNV &other) const;

	/*! \brief The != operator
	 *
	 * \param other the instance to compare
	 * \return boolean value of the comparison operation
	 */
	bool operator!=(const VLNV &other) const;

	// create relative filepath of the vlnv and return it in a QString
	QString createDirPath() const;

	/*! \brief create a QString that contains the vlnv info by using the separator
	* 
	* The string contains the names of the elements.
	* i.e: Vendor: <vendor><separator><library><separator>...
	*/
	QString createString(QString& separator) const;

	/*! \brief Create a QString that contains the vlnv.
	 *
	 * \param separator The separator that is placed between each vlnv field
	 *
	 * \return QString contains the vlnv in one string.
	*/
	QString toString(const QString& separator = QString(":")) const;

	/*! \brief Checks if the vlnv is valid or not
	 *
	 * \return True if the VLNV tag is valid, otherwise false.
	*/
	bool isValid() const;

	/*! \brief Get the VLNV-element specified by column
	 *
	 * \param column Specifies which VLNV element is wanted.
	 *
	 * Function returns the vendor, library, name or version name.
	 * 0: Type
	 * 1: vendor
	 * 2: library
	 * 3: name
	 * 4: version
	 *
	 * \return QString containing the name of the element.
	*/
	QString getElement(int column) const;

	/*! \brief Static function. Convert QString formatted type to enum Type
	 *
	 * \param type QString that holds the type to get
	 * \return IPXactType that corresponds to the given string
	 */
	static VLNV::IPXactType string2Type(const QString &type);

	/*! \brief Static function. Convert IPXactType to QString
	 *
	 * \param type the type thats wanted as a string
	 * \return QString that holds the type without the "spirit:" prefix
	 */
	static QString IPXactType2String(const IPXactType &type);

	/*! \brief Static function. Convert IPXactType to printable QString
	 *
	 * This function doesn't add the "spirit:"-prefix in strings so it can
	 * be used in printing types for example in UI
	 * \param type IPXactType to be converted to QString
	 * \return QString holding the type in user-friendly format
	 */
	static QString type2Print(const IPXactType &type);

    static QString type2Show(const IPXactType &type);

	/*! \brief Is the document identified by this vlnv valid or not.
	 *
	 * \return True if the document is valid
	*/
	bool documentIsValid() const;

	/*! \brief Set the information on the validity of the document.
	 *
	 * \param isValid Defines if the document is valid or not.
	 *
	*/
	void setDocumentValid(bool isValid);

	/*! \brief Clear the vlnv (make all fields empty and type = INVALID)
	 *
	*/
	void clear();
    
private:

	//! \brief The name of the vendor
	QString vendor_;

	//! \brief The name of the library
	QString library_;

	//! \brief The name of the IP block
	QString name_;

	//! \brief The version number of the block
	QString version_;

	//! \brief The type of the document the VLNV-tag belongs to
	IPXactType type_;

	//! \brief Is this component marked as hidden in the library
	bool isHidden_;

	//! \brief Is the document identified by this vlnv a valid one or not.
	bool documentIsValid_;
};

// With this declaration, VLNV can be used as a QVariant.
Q_DECLARE_METATYPE(VLNV)

#endif /* VLNV_H_ */
