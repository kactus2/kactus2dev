/* 
 *
 *  Created on: 30.7.2010
 *      Author: Antti Kamppi
 */

#ifndef ADDRESSBLOCK_H_
#define ADDRESSBLOCK_H_

#include "generaldeclarations.h"
#include "memorymapitem.h"
#include "parameter.h"
#include "memoryblockdata.h"

#include "ipxactmodels_global.h"

#include <QDomNode>
#include <QString>
#include <QMap>
#include <QSharedPointer>
#include <QXmlStreamWriter>
#include <QTextStream>

class Choice;
class RegisterModel;
class MemoryBlockData;
class VendorExtension;

/*! \brief Equals to spirit:addressBlock element in IP-Xact specification.
 *
 * The AddressBlock describes a single contiguous block of memory that is part of
 * a memory map.
 * AddressBlock is a generalization of a class MemoryMapItem which is also the
 * base class for classes Bank and SubspaceMap.
 */
class IPXACTMODELS_EXPORT AddressBlock: public MemoryMapItem {

public:

	/*! \brief The constructor
	 *
	 * \param memoryMapNode A reference to the QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs if mandatory element is missing in this
	 * class or some member class.
	 */
	AddressBlock(QDomNode &memoryMapNode);

	/*! \brief The default constructor.
	 *
	*/
	AddressBlock();

	//! \brief The copy constructor
	AddressBlock(const AddressBlock &other);

	//! \brief The assignment operator
	AddressBlock &operator=(const AddressBlock &other);

	//! \brief The destructor
	~AddressBlock();

	/*! \brief Clone this address block and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit MemoryMapItem.
	 *
	 * \return QSharedPointer<MemoryMapItem> Pointer to the cloned address block.
	*/
	virtual QSharedPointer<MemoryMapItem> clone() const;

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*
	*/
	virtual void write(QXmlStreamWriter& writer);

	/*! \brief Check if the address block is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, 
        QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the address block is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
     *
	 * \return bool True if the state is valid and writing is possible.
	*/
	 virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

	 /*! \brief Check if the address block contains registers with given names.
	 *
	 * Method: 		uniqueRegisterNames
	 * Full name:	AddressBlock::uniqueRegisterNames
	 * Access:		public 
	 *  
	 * The register names are checked against the given list and appended to it. 
	 * If all register names are unique then true is returned.
	 *
	 * \param regNames List of strings to be checked.
	 *
	 * \return True if all register names are unique.
	 */
	 bool uniqueRegisterNames(QStringList& regNames) const;

	 /*! \brief Write the register names and addresses to the given stream.
	 *
	 * Method: 		writeRegisters
	 * Full name:	MemoryMap::writeRegisters
	 * Access:		public 
	 * 
	 * \param stream The stream where the data is written into.
	 * \param offset The offset added to the register addresses.
	 * \param useAddrBlockID If false then containing address block name is prepended to each register name.
	 * \param idString The string which is added to the beginning of names. This can be used e.g. to contain an instance name.
	 *
	*/
	void writeRegisters(QTextStream& stream, quint64 offset, bool useAddrBlockID = false, const QString& idString = QString()) const;

	 /*! \brief Get the access information
	 *
	 * \return Accessibility of the data in the address block
	 */
	General::Access getAccess() const;

	/*! \brief Get the address range of the address block
	 *
	 * \return QString containing the address range.
	 */
	QString getRange() const;

	/*! \brief Get the attributes linked to range element
	 *
	 * \return A const reference to a QMap containing the attributes
	 */
	const QMap<QString, QString>& getRangeAttributes();

	/*! \brief Get the last address contained in the address block.
	 *
	 * \return The last address of the address block.
	*/
	quint64 getLastAddress() const;

	/*! \brief Get the last address contained in the address block in string format.
	 *
	 * \return QString containing the last address contained in the address block.
	*/
	QString getLastAddressStr() const;

	/*! \brief Get the address offset of the last register contained in the address block.
	 * 
	 * Note: If the address block does not contain registers then 0 is returned.
	 * 
	 * \return The address of the last register.
	*/
	quint64 getLastRegisterAddress() const;

	/*! \brief Get the type of usage for the address block
	 *
	 * \return Usage of the address block
	 */
	General::Usage getUsage() const;

	/*! \brief Get the volatile information
	 *
	 * \return boolean value of the volatile element
	 */
	General::BooleanValue getVolatile() const;

	/*! \brief Get the bit width of a row in the address block
	 *
	 * \return unsigned int containing the width of the address block
	 */
	int getWidth() const;

	/*! \brief Get the attributes linked to the width element
	 *
	 * \return A const reference to a QMap containing the attributes for the
	 * width element.
	 */
	const QMap<QString, QString>& getWidthAttributes();

	/*! \brief Get pointer to the list containing the registers.
	 *
	 * \return Pointer to QList containing pointers to the registers.
	*/
	QList<QSharedPointer<RegisterModel> >* getRegisterPointer();

	/*! \brief Set the accessibility of the address block
	 *
	 * \param access Enum holding the access type of the address block
	 */
	void setAccess(General::Access access);

	/*! \brief set the address range of the address block
	 *
	 * \param range QString holding the range value.
	 */
	void setRange(const QString& range);

	/*! \brief Set the attributes linked to range element
	 *
	 * Calling this function deletes the old attributes. If old values must
	 * be saved then function getRangeAttributes() must first be called to
	 * add the old values to the QList given as a parameter for this function.
	 *
	 * \param rangeAttributes A reference to a QMap containing the attributes.
	 */
	void setRangeAttributes(const QMap<QString, QString>& rangeAttributes);

	/*! \brief Set the type of usage for the address block
	 *
	 * \param usage The enum containing the type of usage
	 */
	void setUsage(General::Usage usage);

	/*! \brief Set the volatile setting
	 *
	 * \param volatileSetting boolean value if the addressblock is volatile or
	 * not
	 */
	void setVolatile(const General::BooleanValue volatileSetting);

	/*! \brief Set the bit width of a row in the address block
	 *
	 * \param width The width to be set for the address block
	 */
	void setWidth(int width);

	/*! \brief Set the atributes linked to the width element
	 *
	 * Calling this function deletes the old attributes. If old values must
	 * be saved then function getWidthAttributes() must first be called to
	 * add the old values to the QList given as a parameter for this function.
	 *
	 * \param widthAttributes A reference to a QMap containing the attributes
	 * for the width element.
	 */
	void setWidthAttributes(const QMap<QString, QString>& widthAttributes);

	/*! \brief Get the parameters of the address block
	 *
	 * \return QList containing the attributes
	 */
	const QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Set the parameters for the address block
	 *
	 * Calling this function deletes the old parameters. If old values must
	 * be saved then function getParameters() must first be called to
	 * add the old values to the QList given as a parameter for this function.
	 *
	 * \param parameters QList containing the new parameters
	 */
	void setParameters(QList<QSharedPointer<Parameter> > &parameters);

	/*! \brief Get the list containing the register elements of address block.
	 *
	 * \return QList containing the pointers to the register elements.
	*/
	QList<QSharedPointer<RegisterModel> >& getRegisterData();

	/*! \brief Get the list containing the register elements of address block.
	 *
	 * \return QList containing the pointers to the register elements.
	*/
	const QList<QSharedPointer<RegisterModel> >& getRegisterData() const;

private:

	/*!
	 * MANDATORY (spirit:range)
	 * Range of an address block. Expressed as the number of addressable units.
	 */
	QString range_;

	/*!
	 * OPTIONAL
	 * The attributes linked to the range element in IP-Xact document.
	 * key = name of the attribute
	 * value = value of the attribute
	 */
	QMap<QString, QString> rangeAttributes_;

	/*!
	 * MANDATORY (spirit:width)
	 * The bit width of a row in the address block.
	 */
	int width_;

	/*!
	 * OPTIONAL
	 * The attributes linked to the range element in IP-Xact document.
	 * key = name of the attribute
	 * value = value of the attribute
	 */
	QMap<QString, QString> widthAttributes_;

	/*!
	 * OPTIONAL
	 * Collection of elements that contain further specification of AddressBlock.
	 */
	MemoryBlockData memoryBlockData_;

	/*! \brief OPTIONAL spirit:registerData
	 * Group contains information about the grouping of bits into registers and
	 * fields.
	 */
	QList<QSharedPointer<RegisterModel> > registerData_;

    /*!
	 * OPTIONAL (spirit: vendorExtensions)
	 * Address block vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;

};

#endif /* ADDRESSBLOCK_H_ */
