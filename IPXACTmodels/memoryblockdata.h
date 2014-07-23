/* 
 *
 *  Created on: 18.10.2010
 *      Author: Antti Kamppi
 */

#ifndef MEMORYBLOCKDATA_H_
#define MEMORYBLOCKDATA_H_

#include "generaldeclarations.h"

#include "ipxactmodels_global.h"

#include <QSharedPointer>
#include <QList>
#include <QXmlStreamWriter>

class Parameter;

/*! \brief Equals the spirit:memoryBlockData element in IP-Xact specification.
 *
 * MemoryBlockData is an optional part of AddressBlock and Bank.
 */
class IPXACTMODELS_EXPORT MemoryBlockData {

public:

	/*! \brief The constructor
	 *
	 * \param usage The usage element
	 * \param volatileValue The volatile element
	 * \param access The access element
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
    MemoryBlockData(General::Usage usage,
		General::BooleanValue volatileValue,
    		General::Access access,
			const QList<QSharedPointer<Parameter> >& parameters);

	//! \brief The default constructor
	MemoryBlockData();

	//! \brief Copy constructor
	MemoryBlockData(const MemoryBlockData& other);

	//! \brief Assignment operator
	MemoryBlockData& operator=(const MemoryBlockData& other);

    //! \brief The destructor
	virtual ~MemoryBlockData();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the memory block data is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the memory block data is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

	/*! \brief Get the usage
	 *
	 * \return enum Usage
	 */
	General::Usage getUsage() const;

	/*! \brief Get the volatile value
	 *
	 * \return enum BooleanValue.
	 */
	General::BooleanValue getVolatile() const;

	/*! \brief Get the access information.
	 *
	 * \return enum Access.
	 */
	General::Access getAccess() const;

	/*! \brief Set the usage.
	 *
	 * \param usage The new value.
	 */
	void setUsage(General::Usage usage);

	/*! \brief Set the volatile value.
	 *
	 * \param volatileValue The new value.
	 */
	void setVolatile(const General::BooleanValue volatileValue);

	/*! \brief Set the access.
	 *
	 * \param access The new value.
	 */
	void setAccess(General::Access access);

	/*! \brief Get the parameters of the memoryBlockData.
	 *
	 * \return QList containing pointers to the parameters.
	 */
	const QList<QSharedPointer<Parameter> >& getParameters() const;

	/*! \brief Set the parameters for the memoryBlockData.
	 *
	 * \param parameters QList containing pointers to the parameters.
	 */
	void setParameters(QList<QSharedPointer<Parameter> >& parameters);

	/*! \brief Add a new parameter.
	 *
	 * \param parameter Pointer to the parameter to add.
	 *
	*/
	void addParameter(QSharedPointer<Parameter> parameter);

private:

	/*!
	 * OPTIONAL (spirit:usage)
	 * The type of usage for the address block or bank to which it belongs to.
	 */
	General::Usage usage_;

	/*!
	 * OPTIONAL (spirit:volatile)
	 * Default value = General::BOOL_UNSPECIFIED
	 */
	General::BooleanValue volatile_;

	/*!
	 * OPTIONAL (spirit:access)
	 * Specifies the accessibility of the data in the address block
	 */
	General::Access access_;

	/*!
	 * OPTIONAL (spirit:parameters)
	 * Any additional parameters that describe the address block.
	 */
	QList<QSharedPointer<Parameter> > parameters_;
};

#endif /* MEMORYBLOCKDATA_H_ */
