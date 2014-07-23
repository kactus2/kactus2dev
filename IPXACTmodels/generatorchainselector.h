/* 
 *
 *  Created on: 12.8.2010
 *      Author: Antti Kamppi
 */

#ifndef GENERATORCHAINSELECTOR_H_
#define GENERATORCHAINSELECTOR_H_

#include "generaldeclarations.h"

#include "ipxactmodels_global.h"

#include"vlnv.h"

#include <QString>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>

/*! \brief Equals the spirit:generatorChainSelector element in IP-Xact
 *
 * GeneratorChainSelector defines which generator(s) to invoke based on a
 * selection criteria.
 */
class IPXACTMODELS_EXPORT GeneratorChainSelector {

public:

	/*! \brief The constructor
	 *
	 * \param generatorChainNode A reference to a QDomNode to parse the
	 * information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	GeneratorChainSelector(QDomNode &generatorChainNode);

	//! \brief Copy constructor
	GeneratorChainSelector(const GeneratorChainSelector& other);

	//! \brief Assignment operator
	GeneratorChainSelector& operator=(const GeneratorChainSelector& other);

	/*! \brief The destructor
	 *
	 */
	~GeneratorChainSelector();

	/*! \brief Get the generator chain reference
	 *
	 * \return A VLNV pointer containing the vlnv information to the
	 * generator chain.
	 */
	VLNV getGeneratorChainRef() const;

	/*! \brief Get the group names
	 *
	 * \return QList containing the group names.
	 */
	const QList<QString>& getGroupNames();

	/*! \brief Get the group selector operator
	 *
	 * \return enum GroupSelectorOperator
	 */
	General::GroupSelectorOperator getOperator() const;

	/*! \brief Get the value of the unique operator
	 *
	 * \return boolean value
	 */
	bool getUnique() const;

	/*! \brief Set the generator chain reference
	 *
	 * \param generatorChainRef A pointer to the VLNV tag
	 */
	void setGeneratorChainRef(const VLNV& generatorChainRef);

	/*! \brief Set the group names
	 *
	 * Calling this function will delete the old group names
	 *
	 * \param groupNames QList containing the group names.
	 */
	void setGroupNames(const QList<QString>& groupNames);

	/*! \brief Set the group selection operator
	 *
	 * \param groupOperator the operator
	 */
	void setOperator(General::GroupSelectorOperator groupOperator);

	/*! \brief Set the value of the unique-attribute
	 *
	 * \param unique boolean value.
	 */
	void setUnique(bool unique);

private:

	/*!
	 * OPTIONAL spirit:unique
	 * Attribute that when true, indicates that the generatorChainSelector
	 * shall resolve to a single generator.
	 * default = false
	 */
	bool unique_;

	/*!
	 * OPTIONAL spirit:multipleGroupSelectorOperator
	 * Specifies if the selection applies when one or all the group names match
	 * default = General:OR
	 */
	General::GroupSelectorOperator operator_;

	/*!
	 * MANDATORY spirit:name
	 * An unbounded list of selection names.
	 */
	QList<QString> groupNames_;

	/*!
	 * MANDATORY spirit:generatorChainRef
	 * Specifies a reference to another generator chain description for
	 * inclusion in this generator chain.
	 */
	VLNV generatorChainRef_;
};

#endif /* GENERATORCHAINSELECTOR_H_ */
