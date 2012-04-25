/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#ifndef BANK_H_
#define BANK_H_

#include "generaldeclarations.h"
#include "memorymapitem.h"
#include "memoryblockdata.h"

#include <QList>
#include <QString>
#include <QDomNode>
#include <QSharedPointer>
#include <QXmlStreamWriter>

/*! \brief Equals the spirit:bank element in IP-Xact specification
 *
 * Bank represents a collection of address blocks, banks or subspace maps.
 * Bank is a generalization of a class MemoryMapItem which is the base class
 * also for classes AddressBlock and SubspaceMap.
 */
class Bank: public MemoryMapItem {

public:

	/*! \brief The constructor
	 *
	 *\param memoryMapNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Bank(QDomNode &memoryMapNode);

	//! \brief Copy constructor
	Bank(const Bank &other);

	//! \brief Assignment operator
	Bank &operator=(const Bank &other);

	/*! \brief The destructor
	 *
	 */
	~Bank();

	/*! \brief Clone this bank and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit MemoryMapItem.
	 *
	 * \return QSharedPointer<MemoryMapItem> Pointer to the cloned bank.
	*/
	virtual QSharedPointer<MemoryMapItem> clone() const;

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	virtual void write(QXmlStreamWriter& writer);

	/*! \brief Check if the bank is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the bank is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid() const;

	/*! \brief Get the items stored in the bank.
	 *
	 * \return QList containing pointers to the items. Pointers are base class
	 * MemoryMapItem type so that instances of Banks, SubspaceMaps and
	 * AddressBlocks can be saved in the same data structure.
	 */
	const QList<QSharedPointer<MemoryMapItem> >& getItems();

	/*! \brief Set the items stored in the bank.
	 *
	 * Calling this function deletes the old items stored in the bank. To keep old
	 * values function getItems() must be called to add the old values to the
	 * QList given as a parameter for this function.
	 *
	 * \param items QList containing pointers to the items stored in the bank.
	 */
	void setItems(QList<QSharedPointer<MemoryMapItem> > &items);

	/*! \brief Get the access information
	 *
	 * \return Accessibility of the data in the Bank
	 */
	General::Access getAccess() const;

	/*! \brief Get the type of usage for the Bank
	 *
	 * \return Usage of the Bank
	 */
	General::Usage getUsage() const;

	/*! \brief Get the volatile information
	 *
	 * \return BooleanValue value of the volatile element
	 */
	General::BooleanValue getVolatile() const;

	/*! \brief Set the accessibility of the Bank
	 *
	 * \param access Enum holding the access type of the Bank
	 */
	void setAccess(General::Access access);

	/*! \brief Set the type of usage for the Bank
	 *
	 * \param usage The enum containing the type of usage
	 */
	void setUsage(General::Usage usage);

	/*! \brief Set the volatile setting
	 *
	 * \param volatileSetting boolean value if the Bank is volatile or
	 * not
	 */
	void setVolatile(const General::BooleanValue volatileSetting);

	/*! \brief Get the parameters of the Bank
	 *
	 * \return QList containing the attributes
	 */
	const QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Set the parameters for the Bank
	 *
	 * Calling this function deletes the old parameters. If old values must
	 * be saved then function getParameters() must first be called to
	 * add the old values to the QList given as a parameter for this function.
	 *
	 * \param parameters QList containing the new parameters
	 */
	void setParameters(QList<QSharedPointer<Parameter> > &parameters);

private:

	/*!
	 * Contains the items stored in the Bank.
	 */
	QList<QSharedPointer<MemoryMapItem> > items_;

	/*!
	 * OPTIONAL
	 * Collection of elements that contain further specification of Bank.
	 */
	QSharedPointer<MemoryBlockData> memoryBlockData_;

};

#endif /* BANK_H_ */
