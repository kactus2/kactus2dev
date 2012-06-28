/* 
 *
 *  Created on: 18.8.2010
 *      Author: Antti Kamppi
 */

#ifndef TRANSACTIONALABSTRACTION_H_
#define TRANSACTIONALABSTRACTION_H_

#include "generaldeclarations.h"

#include <common/Global.h>

#include <QSharedPointer>
#include <QList>
#include <QDomNode>
#include <QString>

class TransactionalPort;

/*! \brief Equals the spirit:transactional element within abstractionDefinition
 * in IP-Xact specification.
 *
 * Defines a logical transactional port of the abstraction definition.
 */
class KACTUS2_API TransactionalAbstraction {

public:

	/*! \brief Equals the spirit:transactionalPort element in IP-Xact
	 * specification.
	 *
	 * Contains elements defining constraints associated with a transactional
	 * logical port within an abstractionDefinition.
	 */
	struct TransactionalPort {

		/*!
		 * OPTIONAL spirit:presence
		 * Provides the capability to require or forbid a port to appear in
		 * a busInterface
		 */
		General::Presence presence_;

		/*!
		 * MANDATORY spirit:initiative
		 * Defines the type of access
		 */
		General::Initiative serviceInitiative_;

		/*!
		 * MANDATORY spirit:typeName
		 * An unbounded list that defines the names of the transactional
		 * interface types.
		 */
		QList<QString> typeNames_;

		/*! \brief The constructor
		 *
		 * \param portNode A reference to a QDomNode to parse the information
		 * from.
		 *
		 * Exception guarantee: basic
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this class or one of it's member classes.
		 */
		TransactionalPort(QDomNode& portNode);
	};

	/*! \brief The constructor
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing
	 *
	 * \param transactionalNode A reference to a QDomNode to parse the
	 * information from.
	 */
	TransactionalAbstraction(QDomNode& transactionalNode);

	//! \brief Copy constructor
	TransactionalAbstraction(const TransactionalAbstraction& other);

	//! \brief Assignment operator
	TransactionalAbstraction& operator=(const TransactionalAbstraction& other);

	/*! \brief The destructor
	 *
	 */
	~TransactionalAbstraction();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Get the onMaster element
	 *
	 * \return A pointer to the onMaster instance.
	 */
	TransactionalPort* getOnMaster() const;

	/*! \brief Get the onSlave element
	 *
	 * \return A pointer to the onSlave instance.
	 */
	TransactionalPort* getOnSlave() const;

	/*! \brief Get the qualifier element
	 *
	 * \return A pointer to the qualifier instance.
	 */
	General::Qualifier* getQualifier() const;

	/*! \brief Set the onMaster element
	 *
	 * \param A pointer to the new TransactionalPort instance
	 */
	void setOnMaster(TransactionalPort* onMaster);

	/*! \brief Set the onSlave element
	 *
	 * \param onSlave A pointer to the new TransactionalPort instance.
	 */
	void setOnSlave(TransactionalPort* onSlave);

	/*! \brief Set the qualifier element
	 *
	 * \param qualifier A pointer to the Qualifier instance.
	 */
	void setQualifier(General::Qualifier* qualifier);

private:

	/*! \brief Write the transactional port information with writer.
	 *
	 * \param writer The writer instance used to write the information.
	 * \param port Pointer to the transactional port to write.
	 */
	void writeTransactionalPort(QXmlStreamWriter& writer,
			TransactionalPort* port);

	/*!
	 * OPTIONAL spirit:qualifier
	 * Indicates which type of information this transactional port carries.
	 */
	QSharedPointer<General::Qualifier> qualifier_;

	/*!
	 * OPTIONAL spirit:onMaster
	 * Defines constraints for this port when present in a master bus interface.
	 */
	QSharedPointer<TransactionalPort> onMaster_;

	/*!
	 * OPTIONAL spirit:onSlave
	 * Defines constraints for this port when present in a slave bus interface.
	 */
	QSharedPointer<TransactionalPort> onSlave_;
};

#endif /* TRANSACTIONALABSTRACTION_H_ */
