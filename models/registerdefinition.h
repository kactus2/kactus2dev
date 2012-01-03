/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppi
 */

#ifndef REGISTERDEFINITION_H_
#define REGISTERDEFINITION_H_

#include "generaldeclarations.h"

#include <QString>
#include <QXmlStreamWriter>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>
#include <QMap>

class Reset;
class Field;

/*! \brief Equals the spirit:registerDefinitionGroup element in IP-Xact.
 *
 * Contains additional elements for the register.
 */
class RegisterDefinition {

public:

	/*! \brief The constructor
	 *
	 * \param registerDefNode A reference to a QDomNode to parse the iformation
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	RegisterDefinition(QDomNode& registerDefNode);

	//! \brief Copy constructor
	RegisterDefinition(const RegisterDefinition& other);

	//! \brief Assignment operator
	RegisterDefinition& operator=(const RegisterDefinition& other);

	//! \brief The destructor.
	virtual ~RegisterDefinition();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*
	* Exception guarantee: basic
	* \exception Write_error Occurs if class or one of it's member classes is
	* not valid IP-Xact in the moment of writing.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the register definition is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Get the access.
	 *
	 * \return enum Access.
	 */
    General::Access getAccess() const;

    /*! \brief Get list of the fields.
     *
     * \return QList containing pointers to the fields.
     */
    const QList<QSharedPointer<Field> >& getFields() const;

    /*! \brief Get the reset-element.
     *
     * \return A pointer to the reset element.
     */
    Reset* getReset() const;

    /*! \brief Get the width of the register.
     *
     * \return Width of register.
     */
    unsigned int getSize() const;

    /*! \brief Get the attributes for the size-element.
     *
     * \return QMap containing the attributes.
     */
    const QMap<QString,QString>& getSizeAttributes() const;

    /*! \brief Get the type identifier.
     *
     * \return QString containing the type identifier.
     */
    QString getTypeIdentifier() const;

    /*! \brief Get the volatile value.
     *
     * \return enum BooleanValue.
     */
    General::BooleanValue getVolatile() const;

    /*! \brief Set the access.
     *
     * \param access The new value.
     */
    void setAccess(General::Access access);

    /*! \brief Set the nwe fields for the register.
     *
     * Calling this function will erase the old fields.
     *
     * \param fields QList containing pointers to the fields.
     */
    void setFields(const QList<QSharedPointer<Field> >& fields);

    /*! \brief Set the reset-element of the register.
     *
     * \param reset A pointer to the new reset instance.
     */
    void setReset(Reset* reset);

    /*! \brief Set the width of the register.
     *
     * \param size The new width.
     */
    void setSize(unsigned int size);

    /*! \brief Set the attributes for the size element.
     *
     * Calling this function will erase any old attributes.
     *
     * \param sizeAttributes QMap containing the size attributes.
     */
    void setSizeAttributes(const QMap<QString,QString>& sizeAttributes);

    /*! \brief Set the typeIdentifier for the register.
     *
     * \param typeIdentifier QString containing the identifier.
     */
    void setTypeIdentifier(const QString& typeIdentifier);

    /*! \brief Set the volatileValue.
     *
     * \param volatileValue the new value.
     */
    void setVolatile(General::BooleanValue volatileValue);

private:

	/*! \brief Used to indicate that multiple register contain same information
	 * OPTIONAL spirit:typeIdentifier
	 */
	QString typeIdentifier_;

	/*! \brief Width of the register in bits.
	 * MANDATORY spirit:size
	 */
	unsigned int size_;

	/*! \brief attributes for the size-element.
	 * OPTIONAL
	 */
	QMap<QString, QString> sizeAttributes_;

	/*! \brief Indicates whether tha data is volatile.
	 * OPTIONAL spirit:volatile
	 */
	General::BooleanValue volatile_;

	/*! \brief Indicates the accessibility of the data.
	 * OPTIONAL spirit:access
	 */
	General::Access access_;

	/*! \brief Contains reset-element that defines register value in reset.
	 * OPTIONAL spirit:reset
	 */
	QSharedPointer<Reset> reset_;

	/*! \brief Contains the field-elements for the register.
	 * OPTIONAL spirit:field
	 */
	QList<QSharedPointer<Field> > fields_;

};

#endif /* REGISTERDEFINITION_H_ */
