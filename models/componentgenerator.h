/* 
 *
 *  Created on: 9.8.2010
 *      Author: Antti Kamppi
 */

#ifndef COMPONENTGENERATOR_H_
#define COMPONENTGENERATOR_H_

#include "generator.h"

#include <QList>
#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

/*! \brief Equals the spirit:componentGenerator element in IP-Xact specification
 *
 * Defines a generator that is assigned and may be run on this component.
 * This class is a generalization of class Generator and uses it's services.
 */
class ComponentGenerator: public Generator {

public:

	/*! \brief Used the specify how the generator is  run
	 * Specifies if the generator shall be run once for all instances or
	 * once for each instance of this component.
	 */
	enum Instance {
		INSTANCE,
		ENTITY
	};

	/*! \brief The constructor
	 *
	 * \param generatorNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	ComponentGenerator(QDomNode &generatorNode);

	//! \brief Copy constructor
	ComponentGenerator(const ComponentGenerator &other);

	//! \brief Assignment operator
	ComponentGenerator &operator=(const ComponentGenerator &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~ComponentGenerator();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*
	* Exception guarantee: basic
	* \exception Write_error Occurs if class or one of it's member classes is
	* not valid IP-Xact in the moment of writing.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the component generator is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Get list of the groups for this Component Generator
	 *
	 * \return QList containing the names of the groups this generator belongs
	 * to
	 */
	const QList<QString>& getGroups();

	/*! \brief Get the scope of this Component Generator
	 *
	 * \return enum Instance containing the scope.
	 */
	Instance getScope() const;

	/*! \brief Set the groups for this Component Generator
	 *
	 * Calling this function will delete the old groups.
	 *
	 * \param groups QList containing names of the groups for this generator.
	 */
	void setGroups(QList<QString> &groups);

	/*! \brief Set the scope of this Component Generator.
	 *
	 * \param scope The scope to be set.
	 */
	void setScope(Instance scope);

private:

	/*! \brief OPTIONAL spirit:scope
	 * Indicates if the generator shall be run once for all instances or once
	 * for each instance of this component.
	 */
	Instance scope_;

	/*! \brief OPTIONAL spirit:group
	 * An unbounded list of names used to assign this generator to a group
	 * with other generators.
	 */
	QList<QString> groups_;
};

#endif /* COMPONENTGENERATOR_H_ */
