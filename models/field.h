/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppi
 */

#ifndef FIELD_H_
#define FIELD_H_

#include <common/Global.h>

#include <QString>
#include <QXmlStreamWriter>
#include <QMap>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>

class Parameter;
class EnumeratedValue;

/*! \brief Equals the spirit:field element in IP-Xact specification.
 *
 * Describes a smaller bit field of a register.
 */
class KACTUS2_API Field {

public:

	/*! \brief The constructor
	 *
	 * \param fieldNode A reference to a QDomNode to parse the information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Field(QDomNode& fieldNode);

	//! \brief Copy constructor
	Field(const Field& other);

	//! \brief Assignment operator
	Field& operator=(const Field& other);

	//! \brief The destructor.
	virtual ~Field();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the field is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the field is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

	/*! \brief Get the bit offset.
	 *
	 * \return bit offset.
	 */
    int getBitOffset() const;

    /*! \brief Get the bit width.
     *
     * \return bit width.
     */
    unsigned int getBitWidth() const;

    /*! \brief Get the attributes for the bit width element.
     *
     * \return QMap containing the attributes.
     */
    const QMap<QString,QString>& getBitWidthAttributes() const;

    /*! \brief Get the description of the field.
     *
     * \return QString containing the description.
     */
    QString getDescription() const;

    /*! \brief Get the displayName
     *
     * \return QString containing the name.
     */
    QString getDisplayName() const;

    /*! \brief Get list of the enumeratedValues.
     *
     * \return QList containing pointers to the enumerated values.
     */
    const QList<QSharedPointer<EnumeratedValue> >& getEnumeratedValues() const;

    /*! \brief Get the name of the field
     *
     * \return QString containing the name.
     */
    QString getName() const;

    /*! \brief Get list of the parameters for the field.
     *
     * \return QList containing pointers to the parameters.
     */
    const QList<QSharedPointer<Parameter> >& getParameters() const;

    /*! \brief Get the typeIdentifier.
     *
     * \return QString containing the typeIdentifier.
     */
    QString getTypeIdentifier() const;

    /*! \brief Set the bit offset.
     *
     * \param bitOffset int value of the offset
     */
    void setBitOffset(int bitOffset);

    /*! \brief Set the bit width for the field.
     *
     * \param bitwidth int value of the bit width.
     */
    void setBitWidth(unsigned int bitWidth);

    /*! \brief Set the attributes for the bit width element.
     *
     * \param bitWidthAttributes QMap containing the attributes.
     */
    void setBitWidthAttributes(const QMap<QString,QString>& bitWidthAttributes);

    /*! \brief Set the description for the field.
     *
     * \param description QString containing the description.
     */
    void setDescription(const QString& description);

    /*! \brief Set the displayName for the field.
     *
     * \param displayName QString containing the displayName.
     */
    void setDisplayName(const QString& displayName);

    /*! \brief Set the enumerated values for the field.
     *
     * \param enumeratedValues QList containing pointers to the enumerated
     * values.
     */
    void setEnumeratedValues(const QList<QSharedPointer<EnumeratedValue> >&
    enumeratedValues);

    /*! \brief Set the name for the field.
     *
     * \param name QString containing the name.
     */
    void setName(const QString& name);

    /*! \brief Set the parameters for the field.
     *
     * Calling this function will erase old parameters.
     *
     * \param parameters QList containing pointers to the new parameters
     */
    void setParameters(const QList<QSharedPointer<Parameter> >& parameters);

    /*! \brief Set the typeIdentifier for the field.
     *
     * \param typeIdentifier QString containing the typeIdentifier.
     */
    void setTypeIdentifier(const QString& typeIdentifier);

    QString getId() const;

    void setId(const QString& id);

private:

	/*! \brief Assigns a unique id to the containing element
	 * OPTIONAL spirit:id
	 */
	QString id_;

	/*! \brief Unique name of the field.
	 * MANDATORY spirit:name
	 */
	QString name_;

	/*! \brief A short descriptive name of the field.
	 * OPTIONAL spirit:displayName
	 */
	QString displayName_;

	/*! \brief Description of the field.
	 * OPTIONAL spirit:description
	 */
	QString description_;

	/*! \brief Describes the offset where this bit field starts.
	 * MANDATORY spirit:bitOffset
	 */
	int bitOffset_;

	/*! \brief Indicates multiple field elements
	 * OPTIONAL spirit:typeIdentifier
	 */
	QString typeIdentifier_;

	/*! \brief Width of the field.
	 * MANDATORY spirit:bitWidth
	 */
	unsigned int bitWidth_;

	//! \brief Contains attributes for the bitWidth element.
	QMap<QString, QString> bitWidthAttributes_;

	/*! \brief Contains pointers to the enumerated values.
	 * OPTIONAL spirit:fieldData
	 */
	QList<QSharedPointer<EnumeratedValue> > enumeratedValues_;

	/*! \brief Contains the parameters for th field.
	 * OPTIONAL spirit:parameters
	 */
	QList<QSharedPointer<Parameter> > parameters_;
};

#endif /* FIELD_H_ */
