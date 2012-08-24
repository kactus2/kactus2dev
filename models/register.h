/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#ifndef REGISTER_H_
#define REGISTER_H_

#include "registermodel.h"
#include "field.h"
#include "registerdefinition.h"

#include <common/Global.h>

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QString>
#include <QList>
#include <QSharedPointer>

class AlternateRegister;
class RegisterDefinition;

/*! \brief Equals the spirit:register element in IP-Xact specification.
 *
 * This class is inherited from RegisterModel base class.
 */
class KACTUS2_API Register: public RegisterModel {

public:

	/*! \brief The constructor
	 *
	 * \param registerNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Register(QDomNode& registerNode);

	//! \brief The default constructor.
	Register();

	//! \brief Copy constructor
	Register(const Register& other);

	//! \brief Assignment operator
	Register& operator=(const Register& other);

	//! \brief The destructor.
	virtual ~Register();

	/*! \brief Clone the register and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit register model.
	 *
	 * \return QSharedPointer<RegisterModel> Pointer to the cloned register.
	*/
	virtual QSharedPointer<RegisterModel> clone();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the register is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the register is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid() const;

	/*! \brief Get the addressOffset of the register
	 *
	 * \return QString containing the address offset.
	 */
    QString getAddressOffset() const;

    /*! \brief Get the alternate registers of this register.
     *
     * \return QList containing pointers to the alternate registers.
     */
    const QList<QSharedPointer<AlternateRegister> >& getAlternateRegisters() const;

    /*! \brief Get the dimension of the register.
     *
     * \return The value of the dim-element.
     */
    int getDim() const;

    /*! \brief Set the addressOffset for this register.
     *
     * \param addressOffset QString containing the addressOffset.
     */
    void setAddressOffset(const QString& addressOffset);

    /*! \brief Set the alternate registers for this register.
     *
     * Calling this function will erase previous alternateRegisters.
     *
     * \param alternateRegisters QList containing pointers to the new
     * alternateRegisters.
     */
    void setAlternateRegisters(
		const QList<QSharedPointer<AlternateRegister> >& alternateRegisters);

    /*! \brief Set the dimension of the register.
     *
     * \param dim Value of the dim-element.
     */
    void setDim(int dim);

	/*! \brief Get list of the fields.
     *
     * \return QList containing pointers to the fields.
     */
    const QList<QSharedPointer<Field> >& getFields();

	/*! \brief Get the width of the register.
	 *
	 * \return The number of bits this register contains.
	*/
	unsigned int getWidth() const;

	/*! \brief Get the type identifier of the register.
	 *
	 * \return QString containing the type identifier.
	*/
	QString getTypeIdentifier() const;

	/*! \brief Set the type identifier for the register.
	 *
	 * \param typeIdentifier The type identifier to set.
	 *
	*/
	void setTypeIdentifier(const QString& typeIdentifier);

	/*! \brief Get the width of the register.
	 *
	 * \return The width of the register in bits.
	*/
	unsigned int getSize() const;

	/*! \brief Set the width of the register.
	 *
	 * \param size The width of the register in bits.
	 *
	*/
	void setSize(unsigned int size);

	/*! \brief Get the volatile state of the register.
	 *
	 * \return The volatile state.
	*/
	General::BooleanValue getVolatile() const;

	/*! \brief Set the volatile state for the register.
	 *
	 * \param volatileValue The value to set.
	 *
	*/
	void setVolatile(General::BooleanValue volatileValue);

	/*! \brief Get the access type of the register.
	 *
	 * \return The access type.
	*/
	General::Access getAccess() const;

	/*! \brief Set the access type for the register.
	 *
	 * \param access The access type to set.
	 *
	*/
	void setAccess(General::Access access);

	/*! \brief Get the reset value of the register.
	 *
	 * \return QString containing the register reset value.
	*/
	QString getRegisterValue() const;

	/*! \brief Set the reset value for the register.
	 *
	 * \param registerValue The reset value to set.
	 *
	*/
	void setRegisterValue(const QString& registerValue);

	/*! \brief Get the register's reset mask.
	 *
	 * \return QString containing the register's reset mask.
	*/
	QString getRegisterMask() const;

	/*! \brief Set the register's reset mask.
	 *
	 * \param registerMask The register's reset mask to set.
	 *
	*/
	void setRegisterMask(const QString& registerMask);

private:

	/*! \brief Assigns an unbounded dimension to the register
	 * OPTIONAL spirit:dim
	 */
	int dim_;

	/*! \brief Offset from the base address.
	 * MANDATORY spirit:addressOffset.
	 */
	QString addressOffset_;

	/*! \brief Contains the alternateRegisters for this register
	 * OPTIONAL spirit:alternateRegisters
	 */
	QList<QSharedPointer<AlternateRegister> > alternateRegisters_;

	/*! \brief Contains the registerDefinitionGroup element.
	 * MANDATORY spirit:registerDefinitionGroup
	 */
	RegisterDefinition registerDefinition_;
};

#endif /* REGISTER_H_ */
