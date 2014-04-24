/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#ifndef REGISTERFILE_H_
#define REGISTERFILE_H_

#include "registermodel.h"

#include <common/Global.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QMap>
#include <QSharedPointer>

/*! \brief Equals the spirit:registerFile element in IP-Xact specification.
 *
 * This class is inherited from RegisterModel base class.
 */
class KACTUS2_API RegisterFile: public RegisterModel {

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
	RegisterFile(QDomNode& registerNode);

	//! Copy constructor
	RegisterFile(const RegisterFile& other);

	//! Assignment operator
	RegisterFile& operator=(const RegisterFile& other);

	//! \brief The destructor.
	virtual ~RegisterFile();

	/*! \brief Clone the register file and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit register model.
	 *
	 * \return QSharedPointer<RegisterModel> Pointer to the cloned register file.
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
	virtual void write(QXmlStreamWriter& writer);

	/*! \brief Check if the register file is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the register file is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid() const;

	/*! \brief Get the address offset of the registerFile.
	 *
	 * \return QString containing the offset.
	 */
	QString getAddressOffset() const;

	/*! \brief Get the dimension of the register.
	 *
	 * \return The value of the dim-element.
	 */
	int getDim() const;

	/*! \brief Get the range of the registerFile.
	 *
	 * \return The range of the registerFile.
	 */
	unsigned int getRange() const;

	/*! \brief Get the attributes for the range element.
	 *
	 * \return QMap containing the attributes.
	 */
	const QMap<QString,QString>& getRangeAttributes() const;

	/*! \brief Get the registerData elements of the reigsterFile.
	 *
	 * \return QMap containing pointers to the registers
	 */
	const QMap<QString, QSharedPointer<RegisterModel> >&
	getRegisterData() const;

	/*! \brief Get the typeIdentifier.
	 *
	 * \return QString containing the typeIdentifier.
	 */
	QString getTypeIdentifier() const;

	/*! \brief Set the address offset.
	 *
	 * \param addressOffset QString containing the new offset.
	 */
	void setAddressOffset(const QString& addressOffset);

	/*! \brief Set the dimension of the registerfile.
	 *
	 * \param dim The new value for dim-element.
	 */
	void setDim(int dim);

	/*! \brief Set the range of the registerFile.
	 *
	 * \param range The new range.
	 */
	void setRange(unsigned int range);

	/*! \brief Set the attributes for the range-element.
	 *
	 * \param rangeAttributes QMap containing attributes for the range-element.
	 */
	void setRangeAttributes(const QMap<QString,QString>& rangeAttributes);

	/*! \brief Set the registerData for the registerFile.
	 *
	 * \brief registerData QMap containing pointers to the registers.
	 */
	void setRegisterData(
			const QMap<QString, QSharedPointer<RegisterModel> >& registerData);

	/*! \brief Set the typeIdentifier of the registerFile.
	 *
	 * \param typeIdentifier QString containing the typeIdentifier.
	 */
	void setTypeIdentifier(const QString& typeIdentifier);

private:

	/*! \brief Contains dimensions of a register array.
	 * OPTIONAL spirit:dim
	 */
	int dim_;

	/*! \brief Offset from the base address.
	 * MANDATORY spirit:addressOffset
	 */
	QString addressOffset_;

	/*! \brief Indicates that multiple registerFiles contain same information.
	 * OPTIONAL spirit:typeIdentifier
	 */
	QString typeIdentifier_;

	/*! \brief The range of a registerFile
	 * MANDATORY spirit:range
	 */
	unsigned int range_;

	/*! \brief Attributes for the range element.
	 * OPTIONAL
	 */
	QMap<QString, QString> rangeAttributes_;

	/*! \brief Contains pointers to the registers
	 * OPTIONAL spirit:registerData
	 */
	QMap<QString, QSharedPointer<RegisterModel> > registerData_;
};

#endif /* REGISTERFILE_H_ */
