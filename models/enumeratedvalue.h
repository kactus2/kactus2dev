/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppi
 */

#ifndef ENUMERATEDVALUE_H_
#define ENUMERATEDVALUE_H_

#include <QDomNode>
#include <QString>
#include <QXmlStreamWriter>

/*! \brief Equals the spirit:enumeratedValue element in IP-Xact specification.
 *
 * Descrives a name and value pair for the fiven field.
 */
class EnumeratedValue {

public:

	/*! \brief Matches the values of spirit:usage
	 *
	 */
	enum EnumeratedUsage {
		READ,
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

	//! \brief Copy constructor
	EnumeratedValue(const EnumeratedValue& other);

	//! \brief Assignment operator
	EnumeratedValue& operator=(const EnumeratedValue& other);

    //! \brief The destructor
    virtual ~EnumeratedValue();

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
    unsigned int getValue() const;

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
    void setValue(unsigned int value);

    /*! \brief Write the contents of the class using the writer.
     *
     * Uses the specified writer to write the class contents into file as valid
     * IP-Xact.
     *
     * \param writer A reference to a QXmlStreamWrite instance that is used to
     * write the document into file.
     *
     * Exception guarantee: basic
     * \exception Write_error Occurs if class or one of it's member classes is
     * not valid IP-Xact in the moment of writing.
     */
    void write(QXmlStreamWriter& writer);

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
    static EnumeratedValue::EnumeratedUsage str2Usage(QString& usage,
                EnumeratedUsage defaultValue);

private:

    /*! \brief Defines the software access condition.
     * OPTIONAL spirit:usage
     */
    EnumeratedUsage usage_;

    /*! \brief The name of the enumeratedValue.
     * MANDATORY spirit:name
     */
    QString name_;

    /*! \brief Short descriptive name of the enumeratedValue
     * OPTIONAL spirit:displayName
     */
    QString displayName_;

    /*! \brief Description of the enumeratedValue
     * OPTIONAL spirit:description
     */
    QString description_;

    /*! \brief The value of the enumeratedValue
     * MANDATORY spirit:value
     */
    unsigned int value_;
};

#endif /* ENUMERATEDVALUE_H_ */
