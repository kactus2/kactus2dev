/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppi
 */

#ifndef ENUMERATEDVALUE_H_
#define ENUMERATEDVALUE_H_

#include "ipxactmodels_global.h"

#include <IPXACTmodels/NameGroup.h>

#include <QDomNode>
#include <QString>
#include <QXmlStreamWriter>

class VendorExtension;

/*! \brief Equals the spirit:enumeratedValue element in IP-Xact specification.
 *
 * Describes a name and value pair for the given field.
 */
class IPXACTMODELS_EXPORT EnumeratedValue {

public:

	/*! \brief Matches the values of spirit:usage within spirit:enumeratedValue
	 *
	 */
	enum EnumeratedUsage {
		READ = 0,
		WRITE,
		READWRITE	// default value
	};

	/*! \brief The constructor
	 *
	 * \param enumerationNode A reference to a QDomNode to parse the
	 * information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
    EnumeratedValue(QDomNode& enumerationNode);

	//! \brief The default constructor.
	EnumeratedValue();

	//! \brief Copy constructor
	EnumeratedValue(const EnumeratedValue& other);

	//! \brief Assignment operator
	EnumeratedValue& operator=(const EnumeratedValue& other);

    //! \brief The destructor
    virtual ~EnumeratedValue();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the enumerated value is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the enumerated value is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

	/*! \brief Get the description
     *
     * \return QString containing the description.
     */
    QString getDescription() const;

    /*! \brief Get the displayName
     *
     * \return QString containing the name.
     */
    QString getDisplayName() const;

    /*! \brief Get the name
     *
     * \return QString containing the name.
     */
    QString getName() const;

    /*! \brief Get the usage
     *
     * \return enum EnumeratedUsage.
     */
    EnumeratedUsage getUsage() const;

    /*! \brief Get the value of the enumeratedValue.
     *
     * \return unsigned int the value
     */
	QString getValue() const;

    /*! \brief Set the description
     *
     * \param description QString containing the description.
     */
    void setDescription(const QString& description);

    /*! \brief Set the displayName
     *
     * \param displayName QString containing the name.
     */
    void setDisplayName(const QString& displayName);

    /*! \brief Set the name.
     *
     * \param name QString containing the name
     */
    void setName(const QString& name);

    /*! \brief Set the usage
     *
     * \param usage The new value.
     */
    void setUsage(EnumeratedUsage usage);

    /*! \brief Set the value of the enumeratedValue
     *
     * \param value The new value.
     */
	void setValue(const QString& value);

    /*! \brief Static function that translates enumeratedUsage to QString
     *
     * \param usage The usage to be translated to QString
     *
     * \return QString containing the matching usage.
     */
    static QString usage2Str(EnumeratedValue::EnumeratedUsage usage);

    /*! \brief Static function that translates QString to enumeratedUsage
     *
     * \param usage QString that is translated into enumeratedUsage
     * \param defaultValue The value that is returned if the string doesn't
     * match any usage.
     *
     * \return EnumeratedValue that matches the string of defaultValue.
     */
    static EnumeratedValue::EnumeratedUsage str2Usage(QString const& usage,
                EnumeratedUsage defaultValue);

private:

    /*! \brief Defines the software access condition.
     * OPTIONAL spirit:usage
     */
    EnumeratedUsage usage_;

	//! \brief Contains the name, display name and description of the enumerated value.
	NameGroup nameGroup_;

    /*! \brief The value of the enumeratedValue
     * MANDATORY spirit:value
     */
    QString value_;

    /*!
	 * OPTIONAL (spirit: vendorExtensions)
	 * Enumerated value vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;
};

#endif /* ENUMERATEDVALUE_H_ */
