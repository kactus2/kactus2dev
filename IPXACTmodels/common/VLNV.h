//-----------------------------------------------------------------------------
// File: vlnv.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 23.6.2010
//
// Description:
// A class to hold VLNV information of a single IP-XACT document.
//-----------------------------------------------------------------------------

#ifndef VLNV_H
#define VLNV_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QVector>

#include <QDomNode>

//-----------------------------------------------------------------------------
//! A class to hold VLNV information of a single IP-XACT document.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT VLNV
{
public:

	//! Type is used to tell the type of IP-XACT document.
	enum IPXactType
    {
        BUSDEFINITION = 0,      // ipxact:busDefinition
        COMPONENT,				// ipxact:component
        DESIGN, 				// ipxact:design
        GENERATORCHAIN,         // ipxact:generatorChain
        ABSTRACTOR,             // ipxact:abstractor
        DESIGNCONFIGURATION,    // ipxact:designConfiguration
        ABSTRACTIONDEFINITION,  // ipxact:abstractionDefinition
        COMDEFINITION,          // kactus2:comDefinition
        APIDEFINITION,          // kactus2:apiDefinition
		INVALID                 // invalid type
    };

  	/*! The default constructor
	 *
	 * Constructs an invalid VLNV instance.
	*/
	VLNV();

	/*! The Constructor.
	 *
	 *      @param [in] type    The type of the VLNV document (design, component...).
	 *      @param [in] vendor  The name of the vendor.
	 *      @param [in] library The name of the library.
	 *      @param [in] name    The name of the item.
	 *      @param [in] version The version number of the item.
	 */
	VLNV(QString const& type, QString const& vendor, QString const& library,
		QString const& name, QString const& version);

	/*! Constructor that takes type parameter as enum
	 *
	 * This constructor uses enum IPXactType to define the type of the document.
     *
	 *      @param [in] type    IPXactType type parameter defining document type
	 *      @param [in] vendor  The name of the vendor.
	 *      @param [in] library The name of the library.
	 *      @param [in] name    The name of the item.
	 *      @param [in] version The version number of the item.
	 */
	VLNV(IPXactType const& type, QString const& vendor, QString const& library,
			QString const& name, QString const& version);

	/*! The constructor.
	 *
	 *      @param [in] type The document type for the vlnv.
	 *      @param [in] parseStr The string which is parsed to find the different vlnv fields.
	 *      @param [in] separator The character used to separate the different vlnv fields in the string.
	 *
	*/
	VLNV(IPXactType const& type, QString const& parseStr, QString const& separator = QString(":"));

	/*! Copy constructor
	 *
	 *      @param [in] other The VLNV to be copied.
	 */
	VLNV(VLNV const& other);

	//! The destructor
	~VLNV();

    /*! Get the vendor of the VLNV.
	 *
	 *      @return The name of the vendor.
	 */
	QString getVendor() const;

	/*! Set the vendor for the vlnv.
	 *
	 *      @param [in] vendor The vendor name to set.
	*/
	void setVendor(QString const& vendor);

	/*! Get the library of the VLNV.
	 *
	 *      @return The name of the library
	 */
	QString getLibrary() const;

	/*! Set the library for the vlnv.
	 *
	 *      @param [in] library The library name
	*/
	void setLibrary(QString const& library);

	/*! Get the name of the IP item in VLNV.
	 *
	 *      @return The name of the IP item in VLNV.
	 */
	QString getName() const;

	/*! Set the name for the VLNV.
	 *
	 *      @param [in] name The new name.
	*/
	void setName(QString const& name);

	/*! Get the version of the VLNV.
	 *
	 *      @return the The version of the VLNV.
	 */
	QString getVersion() const;

	/*! Set the version of the VLNV.
	 *
	 *      @param [in] version The new version.
	*/
	void setVersion(QString const& version);

	/*! Get the type of the document the VLNV belongs to.
	 *
	 *      @return IPXactType which tells the document type.
	 */
	IPXactType getType() const;

	/*! Set the document type for the vlnv.
	 *
	 *      @param [in] type Specifies the type to set.
	*/
	void setType(IPXactType type);
    
	/*! Get the type the VLNV belongs to in a QString.
	 *
	 *      @return The XML document element type.
	 */
	QString getTypestr() const;

    /*!
     *  Checks if the VLNV is empty.
     *
     *      @returns True if all VLNV fields are empty, otherwise false.
     */
    bool isEmpty() const;

    /*! Clear the vlnv (make all fields empty and type = INVALID)
	 *
	*/
	void clear();

	/*! Checks if the vlnv is valid or not.
	 *
	 *      @return True, if the VLNV tag is valid, otherwise false.
	*/
	bool isValid() const;

	/*! Check if the vlnv is valid.
	 *
	 *      @param [in] errorList The list to add the possible error messages to.
	 *      @param [in] parentIdentifier String from parent to help to identify the location of the error.
	 *
	 *      @return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QVector<QString>& errors, QString const& parentIdentifier) const;
    
	/*! Create a QString that contains the vlnv.
	 *
	 *      @param [in] separator The separator that is placed between each vlnv field
	 *
	 *      @return QString contains the vlnv in one string.
	*/
	QString toString(QString const& separator = QString(":")) const;

	//! The assignment operator for the class
	VLNV &operator=(const VLNV &other);

	/*! The < operator for comparison of the VLNV info
	 *
	 *      @param [in] other the instance to compare
     *
	 *      @return boolean value of the comparison
	 */
	bool operator<(const VLNV &other) const;

	/*! The > operator for comparison of the VLNV info
	 *
	 *      @param [in] other the instance to compare
     *
	 *      @return the boolean value of the comparison
	 */
	bool operator>(const VLNV &other) const;

	/*! The == comparison operator
	 *
	 *      @param [in] other The other VLNV instance to be compared
     *
	 *      @return boolean value of the comparison operation
	 */
	bool operator==(const VLNV &other) const;

	/*! The != operator
	 *
	 *      @param [in] other the instance to compare
     *
	 *      @return boolean value of the comparison operation
	 */
	bool operator!=(const VLNV &other) const;

	/*! Static function. Convert QString formatted type to enum Type
	 *
	 *      @param [in] type QString that holds the type to get
	 *      @return IPXactType that corresponds to the given string
	 */
	static VLNV::IPXactType string2Type(QString const& type);

    /*! Static function. Converts type to QString.
	 *
	 *      @param [in] type QString that holds the type to get
     *
	 *      @return IPXactType that corresponds to the given string
	 */
    static QString IPXactType2String(const IPXactType &type);

    /*! Parse a vlnv tag from the attributes in the node
    *
    * The node parameter must have attributes matching a vlnv tag
    *
    *      @param [in] node A reference to a QDomNode to parse the vlnv from.
    *
    *      @return The new vlnv instance.
    */
    static VLNV createVLNV(const QDomNode& node, IPXactType type);
   
private:

	//! The name of the vendor
	QString vendor_;

	//! The name of the library
	QString library_;

	//! The name of the IP block
	QString name_;

	//! The version number of the block
	QString version_;

	//! The type of the document the VLNV-tag belongs to
	IPXactType type_;
};

// With this declaration, VLNV can be used as a QVariant.
Q_DECLARE_METATYPE(VLNV);

#endif /* VLNV_H */
