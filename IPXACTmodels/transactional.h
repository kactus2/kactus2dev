/* 
 *
 *  Created on: 3.8.2010
 *      Author: Antti Kamppi
 */

#ifndef TRANSACTIONAL_H_
#define TRANSACTIONAL_H_

#include "generaldeclarations.h"

#include "ipxactmodels_global.h"

#include <QDomNode>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class ServiceTypeDef;

/*! \brief Equals the spirit:transactional element in IP-Xact specification
 *
 * Defines a physical transactional port of the component, which implements or
 * uses a service. A service can be implemented with functions or methods.
 */
class IPXACTMODELS_EXPORT Transactional {

public:

	/*! \brief Defines the port type expressed in the default language
	 *
	 */
	struct TransTypeDef {

		/*!
		 * MANDATORY spirit:typeName
		 * Defines the port type
		 */
		QString typeName_;

		/*!
		 * OPTIONAL spirit:constrained
		 * Indicates that the port type definition has contrained the number
		 * of bits in the vector
		 * default = false
		 */
		bool constrained_;

		/*!
		 * OPTIONAL spirit:typeDefinition
		 * Contains a language specific reference to where the given type is
		 * actually defined
		 */
		QList<QString> typeDefinitions_;

		/*! \brief The constructor
		 *
		 * \param transTypeNode A reference to a QDomNode to parse the
		 * information from.
		 *
		 * Exception guarantee: basic
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this class or one of it's member classes.
		 */
		TransTypeDef(QDomNode &transTypeNode);
	};

	/*! \brief The constructor
	 *
	 * param transactionalNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Transactional(QDomNode &transactionalNode);

	//! \brief Copy constructor
	Transactional(const Transactional &other);

	//! \brief Assignment operator
	Transactional &operator=(const Transactional &other);

	/*! \brief The destructor
	 *
	 */
	~Transactional();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Get the allLogicalInitiativesAllowed setting
	 *
	 * \return boolean value of the allLogicalInitiativesAllowed
	 */
	bool getAllLogicalInitiativesAllowed() const;

	/*! \brief Get the number of max connections
	 *
	 * \return number of maximum supported connections to this port. Number 0
	 * indicates an unbounded number of legal connections.
	 */
	unsigned int getMaxConnections() const;

	/*! \brief Get the minimum number of connections for this port
	 *
	 * \return Minimum number of connections for this port.
	 */
	unsigned int getMinConnections() const;

	/*! \brief Get the service initiative
	 *
	 * \return enum Initiative containing the service initiative
	 */
	General::Initiative getServiceInitiative() const;

	/*! \brief Get the list of serviceTypeDefs for this port
	 *
	 * \return QList containing pointers for the ServiceTypeDefs
	 */
	const QList<QSharedPointer<ServiceTypeDef> >& getServiceTypeDefs();

	/*! \brief Get a pointer to this port's TransTypeDef
	 *
	 * \return A pointer to this port's TransTypeDef
	 */
	TransTypeDef *getTransTypeDef() const;

	/*! \brief Set the value for the allLogicalInitiativesAllowed
	 *
	 * \param allLogicalInitiativesAllowed The value to be set
	 */
	void setAllLogicalInitiativesAllowed(bool &allLogicalInitiativesAllowed);

	/*! \brief Set the maximum connections for this port
	 *
	 * \param maxConnections The number of max connections. 0 indicates an
	 * unbounded number of max connections.
	 */
	void setMaxConnections(unsigned int maxConnections);

	/*! \brief Set the minimum number of connections for this port
	 *
	 * \param minConnections The minimum number of connections.
	 */
	void setMinConnections(unsigned int minConnections);

	/*! \brief Set the service initiative for this port
	 *
	 * \param serviceInitiative The service initiative to be set
	 *
	 * \exception std::invalid_argument Occurs if the given parameter is
	 * General::NONE
	 */
	void setServiceInitiative(General::Initiative &serviceInitiative);

	/*! \brief Set the serviceTypeDefs for this port
	 *
	 * Calling this function will delete the old serviceTypeDefs.
	 *
	 * \param serviceTypeDefs QList containing pointers to the new
	 * serviceTypeDefs
	 */
	void setServiceTypeDefs(const
			QList<QSharedPointer<ServiceTypeDef> > &serviceTypeDefs);

	/*! \brief Set thr TransTypeDef for this port
	 *
	 * Calling this function will delete the old transTypeDef
	 *
	 * \param transTypeDef A pointer to the new transTypeDef.
	 */
	void setTransTypeDef(TransTypeDef *transTypeDef);

private:

	/*!
	 * OPTIONAL spirit:transTypeDef
	 * Defines the port type expressed in the default language for this port.
	 */
	QSharedPointer<TransTypeDef> transTypeDef_;

	/*!
	 * OPTIONAL
	 * Defines whether the port may be mapped to a port in an
	 * abstractionDefinition with a different initiative.
	 */
	bool allLogicalInitiativesAllowed_;

	/*!
	 * OPTIONAL spirit:maxConnections
	 * Indicates the maximum number of connections that this port supports
	 * 0 value indicates unbounded number of legal connections
	 * default = 0
	 */
	unsigned int maxConnections_;

	/*! OPTIONAL spirit:minConnections
	 * Indicates minimum number of connections that this port supports
	 * default = 1
	 */
	unsigned int minConnections_;

	/*!
	 * MANDATORY spirit:initiative
	 * Defines the type of access
	 */
	General::Initiative serviceInitiative_;

	/*!
	 * OPTIONAL spirit:serviceTypeDefs
	 *  Defines a single service type definition.
	 */
	QList<QSharedPointer<ServiceTypeDef> > serviceTypeDefs_;

};

#endif /* TRANSACTIONAL_H_ */
