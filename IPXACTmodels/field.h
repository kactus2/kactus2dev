/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppi
 */

#ifndef FIELD_H_
#define FIELD_H_

#include "generaldeclarations.h"
#include "ipxactmodels_global.h"
#include "writevalueconstraint.h"

#include <IPXACTmodels/NameGroup.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <QString>
#include <QXmlStreamWriter>
#include <QMap>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>

class Choice;
class Parameter;
class EnumeratedValue;
class VendorExtension;

/*! \brief Equals the spirit:field element in IP-Xact specification.
 *
 * Describes a smaller bit field of a register.
 */
class IPXACTMODELS_EXPORT Field 
{

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

	//! \brief The default constructor.
	Field();

	/*! \brief The constructor
	 *
	 * \param volatileValue The volatile value to set.
	 * \param access The access type to set.
	 *
	*/
	Field(General::BooleanValue volatileValue, General::Access access);

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
	 * \param registerSize The size of the containing register, field must not extend beyond this.
     * \param componentChoices  Choices in the containing component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(unsigned int registerSize, QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
        QStringList& errorList, const QString& parentIdentifier) const;

	/*! \brief Check if the field is in a valid state.
	 * 
	 * \param registerSize The size of the containing register, field must not extend beyond this.
     * \param componentChoices  Choices in the containing component.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(unsigned int registerSize,
        QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

	/*! \brief Get the bit offset.
	 *
	 * \return bit offset.
	 */
    int getBitOffset() const;

    /*!
     *  Set the expression for bit offset.
     *
     *      @param [in] expression      The expression for the bit offset.
     */
    void setBitOffsetExpression(QString const& expression);

    /*!
     *  Get the bit offset expression.
     *
     *      @return The expression in bit offset.
     */
    QString getBitOffsetExpression();

    /*!
     *  Check if the field has an expression in bit offset.
     *
     *      @return True, if there is an expression, otherwise false.
     */
    bool hasBitOffsetExpression();

    /*!
     *  Remove the expression from bit offset.
     */
    void removeBitOffsetExpression();

    /*!
     *  Set the expression for field presence.
     *
     *      @param [in] expression  The expression for field presence.
     */
    void setIsPresentExpression(QString const& expression);

    /*!
     *  Get the expression for field presence.
     *
     *      @return The expression for field presence.
     */
    QString getIsPresentExpression();

    /*!
     *  remove the expression from field presence.
     */
    void removeIsPresentExpression();

    /*!
     *  Check if the field has an expression for presence.
     *
     *      @return True, if there is an expression, otherwise false.
     */
    bool hasIsPresentExpression();

    /*! \brief Get the bit width.
     *
     * \return bit width.
     */
    unsigned int getBitWidth() const;

    /*!
     *  Set the expression for bit width.
     *
     *      @param [in] expression   The expression to be set.
     */
    void setBitWidthExpression(QString const& expression);

    /*!
     *  Get the bit width expression.
     */
    QString getBitWidthExpression();

    /*!
     *  Remove the bit width expression.
     */
    void removeBitWidthExpression();

    /*!
     *  Check if the field has an expression as bit width.
     *
     *      @return     True, if there is an expression, otherwise false.
     */
    bool hasBitWidthExpression();

	/*! \brief Get the MSB of the field.
	 *
	 * \return The MSB bit of the field.
	*/
	int getMSB() const;

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

	/*! \brief Get list of the enumeratedValues.
     *
     * \return QList containing pointers to the enumerated values.
     */
	QList<QSharedPointer<EnumeratedValue> >& getEnumeratedValues();

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

    /*! \brief Get the id of the field.
     *
     *
     * \return QString containing the field's id.
    */
    QString getId() const;

    /*! \brief Set the id for the field.
     *
     * \param id The id to set for the field.
     *
    */
    void setId(const QString& id);

	/*! \brief Get the volatile state of the field.
	 *
	 * \return The volatile state.
	*/
	bool getVolatile() const;

	/*! \brief Set the volatile state for the field.
	 *
	 * \param volatileValue The volatile state to set.
	 *
	*/
	void setVolatile(bool volatileValue);

	/*! \brief Get the access type of the field.
	 *
	 * \return The access type of the field.
	*/
	General::Access getAccess() const;

	/*! \brief Set the access type for the field.
	 *
	 * \param access The access type to set.
	 *
	*/
	void setAccess(General::Access access);

	/*! \brief Get the modified write value setting of the field.
	 *
	 * \return The modified write value setting.
	*/
	General::ModifiedWrite getModifiedWrite() const;

	/*! \brief Set the modified write value setting for the field.
	 *
	 * \param modWriteValue The value to set.
	 *
	*/
	void setModifiedWrite(const General::ModifiedWrite modWriteValue);

	/*! \brief Get the read action setting of the field.
	 *
	 * \return The read action setting.
	*/
	General::ReadAction getReadAction() const;

	/*! \brief Set the read action setting for the field.
	 *
	 * \param readAction The value to set.
	 *
	*/
	void setReadAction(const General::ReadAction readAction);

	/*! \brief Get the testable setting of the field.
	 *
	 * \return The testable setting.
	*/
	bool getTestable() const;

	/*! \brief Set the testable setting for the field.
	 *
	 * \param testable The value to set.
	 *
	*/
	void setTestable(bool testable);

	/*! \brief Get the test constraint setting of the field.
	 *
	 * \return The test constraint of the field.
	*/
	General::TestConstraint getTestConstraint() const;

	/*! \brief Set the test constraint for the field.
	 *
	 * \param testContraint The test constraint to set.
	 *
	*/
	void setTestConstraint(const General::TestConstraint testContraint);

	/*! \brief Get pointer to the write constraint of the field.
	 *
	 * \return Pointer to the write constraint.
	*/
	const QSharedPointer<WriteValueConstraint> getWriteConstraint() const;

	/*! \brief Get pointer to the write constraint of the field.
	 *
	 * \return Pointer to the write constraint.
	*/
	QSharedPointer<WriteValueConstraint> getWriteConstraint();

private:

    /*!
     *  Parse the vendor extension from a DOM node.
     *
     *      @param [in] fieldNode   The DOM node containing all the vendor extensions.
     */
    void parseVendorExtensions(QDomNode const& fieldNode);

    /*!
     *  Creates a vendor extension for expression in bit offset.
     *
     *      @param [in] expression  The initial expression to set.
     */
    void createOffsetExpressionExtension(QString const& expression);

    void createIsPresentExpressionExtension(QString const& expression);

    /*!
     *  Copies vendor extensions from another field.
     *
     *      @param [in] other       The field to copy the extension from.
     */
    void copyVendorExtensions(const Field & other);

	/*! \brief Assigns a unique id to the containing element
	 * OPTIONAL spirit:id
	 */
	QString id_;

	//! \brief Contains the name, display name and description of field.
	NameGroup nameGroup_;

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

	//! \brief Contains the volatile value for the field.
	bool volatile_;

	//! \brief Contains the access type of the field.
	General::Access access_;

	//! \brief Contains the modified write setting for the field.
	General::ModifiedWrite modifiedWrite_;

	//! \brief Contains the read action setting for the field.
	General::ReadAction readAction_;

	//! \brief Contains the testable setting for the field.
	bool testable_;

	//! \brief Contains the test constraint setting for the field.
	General::TestConstraint testConstraint_;

	//! \brief Pointer to the write value constraint for the field.
	QSharedPointer<WriteValueConstraint> writeConstraint_;

    /*!
	 * OPTIONAL (spirit: vendorExtensions)
	 * Field vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;

    //! The vendor extension for offset expression.
    QSharedPointer<Kactus2Value> offsetExpression_;

    //! The vendor extension for is present expression.
    QSharedPointer<Kactus2Value> isPresentExpression_;
};

#endif /* FIELD_H_ */
