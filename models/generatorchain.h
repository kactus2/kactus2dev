/* 
 *
 *  Created on: 11.8.2010
 *      Author: Antti Kamppi
 */

#ifndef GENERATORCHAIN_H_
#define GENERATORCHAIN_H_

#include "generaldeclarations.h"
#include "librarycomponent.h"
#include "choice.h"

#include <QList>
#include <QString>
#include <QDomDocument>
#include <QDomNode>
#include <QSharedPointer>
#include <QFile>
#include <QMap>

class Generator;
class GeneratorChainSelector;

/*! \brief Equals the spirit:generatorChain element in IP-Xact specification.
 *
 * Class GeneratorChain is a generalization of from base class LibraryComponent.
 * GeneratorChain describes a single generator chain.
 */
class GeneratorChain: public LibraryComponent {

public:

	/*! \brief Equals the spirit:componentGeneratorSelector element
	 *
	 * Selects a component generator or a list of component generators based
	 * on the assigned group name.
	 */
	struct ComponentGeneratorSelector {

		/*!
		 * OPTIONAL spirit:multipleGroupSelectionOperator
		 * Specifies when the selection applies.
		 * default = General::OR
		 */
		General::GroupSelectorOperator operator_;

		/*!
		 * MANDATORY spirit:name
		 * An unbounded list of selection names.
		 */
		QList<QString> groupNames_;

		/*! \brief The constructor
		 *
		 * \param selectorNode A reference to a QDomDocument to parse the
		 * information from.
		 *
		 * Exception guarantee: basic
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this class or one of it's member classes.
		 */
		ComponentGeneratorSelector(QDomNode &selectorNode);
	};

	/*! \brief The constructor
	 *
	 * \param generatorChainNode A reference to a QDomNode to parse the
	 * information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	GeneratorChain(QDomDocument& doc);

	//! \brief Copy constructor
	GeneratorChain(const GeneratorChain &other);

	//! \brief Assignment operator
	GeneratorChain &operator=(const GeneratorChain &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~GeneratorChain();

	/*! \brief Clone this generator chain and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit LibraryComponent.
	 *
	 * \return QSharedPointer<LibraryComponent> Pointer to the cloned generator chain.
	*/
	virtual QSharedPointer<LibraryComponent> clone() const;

	/*! \brief Write the document contents into file.
	*
	* Prints all information stored in this document into specified file
	* generating a valid IP-Xact document.
	*
	* \param file A reference to QFile instance representing the file to write
	* the data into.
	*
	* \exception Write_error Occurs if the class or one of it's subclasses is
	* not valid IP-Xact at the moment of writing.
	*/
	virtual void write(QFile& file);

	/*! \brief Check the validity of the generator chain.
	 * 
	 * This function should be used to check if the generator chain is in valid state
	 * before writing the xml to the disk.
	 * 
	 * \param errorList The error messages of the detected errors are appended to
	 * this list.
	 *
	 * \return bool True if the generator chain was valid.
	*/
	virtual bool isValid(QStringList& errorList) const;

	/*! \brief Check the validity of the generator chain.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid() const;

	/*! \brief Set the vlnv
	 *
	 * \param vlnv Reference to the vlnv to set
	 *
	*/
	virtual void setVlnv(const VLNV& vlnv);

	/*! \brief Get the list of the names to which this chain belongs.
	 *
	 * \return QList containing the names.
	 */
	const QList<QString>& getChainGroups();

	/*! \brief Get list of the choices for this generator chain.
	 *
	 * \return QList containing pointers to the choices
	 */
        const QList<QSharedPointer<Choice> >& getChoices();

	/*! \brief Get list of the component generator selectors.
	 *
	 * \return QList containing pointers to the component generator selectors
	 */
	const QList<QSharedPointer<ComponentGeneratorSelector> >&
	getCompGenSelectors();

	/*! \brief Get the display name
	 *
	 * \return QString containing the display name.
	 */
	QString getDisplayName() const;

	/*! \brief Get list of the generator chain selectors
	 *
	 * \return QList containing pointers to the generator chain selectors.
	 */
	const QList<QSharedPointer<GeneratorChainSelector> >&
        getGenChainSelectors();

	/*! \brief Get list of the generators in this generator chain
	 *
	 * \return QList containing pointers to the generators.
	 */
	const QList<QSharedPointer<Generator> >& getGenerators();

	/*! \brief Get the value of the hidden-attribute
	 *
         * \return boolean value.
	 */
	bool getHidden() const;

	/*! \brief Get the attributes of the generatorChain.
	 *
	 * \return QMap containing the attributes.
	 */
	const QMap<QString, QString>& getAttributes();

	/*! \brief Set the attributes for the generatorChain.
	 *
	 * Calling this function will erase old attributes.
	 *
	 * \param attributes QMap containing the attributes for the generatorChain.
	 */
	void setAttributes(const QMap<QString, QString>& attributes);

	/*! \brief Set the chain groups
	 *
	 * \param chainGroups QList containing the names of the groups.
	 */
	void setChainGroups(const QList<QString> &chainGroups);

	/*! Set the choices for this generator chain
	 *
	 * Calling this function will delete the old generator chains.
	 *
	 * \param choices QList containing pointers to the choices.
	 */
        void setChoices(const QList<QSharedPointer<Choice> > &choices);

	/*! \brief Set the component generator selectors
	 *
	 * Calling this function will delete old component generator selectors.
	 *
	 * \param compGenSelectors QList containing pointers to the component
	 * generator selectors
	 */
	void setCompGenSelectors(const QList<QSharedPointer<ComponentGeneratorSelector> >
	&compGenSelectors);

	/*! \brief Set the display name
	 *
	 * \param displayName QString containing the display name.
	 */
	void setDisplayName(const QString &displayName);

	/*! \brief Set the generator chain selectors
	 *
	 * Calling this function will delete the old generator chain selectors
	 *
	 * \param genChainSelectors QList containing pointers to the new
	 * generator chain selectors.
	 */
	void setGenChainSelectors(const
			QList<QSharedPointer<GeneratorChainSelector> > &genChainSelectors);

	/*! \brief Set the generators
	 *
	 * Calling this function will delete the old generators.
	 *
	 * \param generators QList containing pointers to the new generators.
	 */
	void setGenerators(const QList<QSharedPointer<Generator> > &generators);

	/*! \brief Set the value of the hidden-attribute.
	 *
	 * \param hidden boolean value.
	 */
	void setHidden(bool &hidden);

	/*! \brief Get QStringList containing the files needed by generatorChain.
	 *
	 * \return QStringList containing filepaths to files that this
	 * generatorChain needs.
	 */
	virtual const QStringList getDependentFiles() const;

	/*! \brief Get list of the VLNVs this generatorChain needs.
	 *
	 * \return List containing VLNVs of other generatorChains that this
	 * generatorChain needs.
	 */
	virtual const QList<VLNV> getDependentVLNVs() const;

private:

	/*!
	 * OPTIONAL spirit:hidden
	 * Attribute for the spirit:generatorChain element
	 * Specifies if this generator chain is presented to the user of a DE or
	 * not.
	 * default = false
	 */
	bool hidden_;

	/*!
	 * OPTIONAL spirit:displayName
	 * Contains the name of the generator chain to be displayed in the DE
	 */
	QString displayName_;

	/*!
	 * OPTIONAL spirit:generatorChainSelector
	 * Selection for selecting on or more generator chains or a reference to
	 * another generator chain.
	 */
	QList<QSharedPointer<GeneratorChainSelector> > genChainSelectors_;

	/*!
	 * OPTIONAL spirit:componentGeneratorSelector
	 * Selection criteria for selecting one or more component generators.
	 */
	QList<QSharedPointer<ComponentGeneratorSelector> > compGenSelectors_;

	/*!
	 * OPTIONAL spirit:generator
	 * Defines the generator
	 */
	QList<QSharedPointer<Generator> > generators_;

	/*!
	 * OPTIONAL spirit:chainGroup
	 * An unbounded list of names to which this chain belongs.
	 */
	QList<QString> chainGroups_;

	/*!
	 * OPTIONAL spirit:choices
	 * Specifies multiple enumerated lists.
	 */
	QList<QSharedPointer<Choice> > choices_;

	//! \brief Contains the attributes for the generatorChain.
	QMap<QString, QString> attributes_;
};

#endif /* GENERATORCHAIN_H_ */
