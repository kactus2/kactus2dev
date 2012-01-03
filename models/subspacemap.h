/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#ifndef SUBSPACEMAP_H_
#define SUBSPACEMAP_H_

#include "memorymapitem.h"
#include "generaldeclarations.h"
#include "parameter.h"

#include <QDomNode>
#include <QList>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class parameter;

/*! \brief Equals the spirit:subspaceMap element in IP-Xact specification
 *
 * SubspaceMap maps the address space of a master interface from an opaque bus
 * bridge into the memory map.
 */
class SubspaceMap: public MemoryMapItem {

public:

	/*! \brief The constructor
	 *
	 * \param memoryMapNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	SubspaceMap(QDomNode &memoryMapNode);

	//! \brief Copy constructor
	SubspaceMap(const SubspaceMap &other);

	//! \brief Assignment operator
	SubspaceMap &operator=(const SubspaceMap &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~SubspaceMap();

	/*! \brief Clone this subspace map and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit MemoryMapItem.
	 *
	 * \return QSharedPointer<MemoryMapItem> Pointer to the cloned subspace map.
	*/
	virtual QSharedPointer<MemoryMapItem> clone() const;

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*
	* Exception guarantee: basic
	* \exception Write_error Occurs if class or one of it's member classes is
	* not valid IP-Xact in the moment of writing.
	*/
	virtual void write(QXmlStreamWriter& writer);

	/*! \brief Check if the sub space map is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Get the list of parameters for the subSpaceMap
	 *
	 * \return QList containing pointers to the parameters
	 */
	const QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Set the parameters for the SubspaceMap
	 *
	 * Calling this function deletes the old parameters. If old values must
	 * be saved then function getParameters() must first be called to
	 * add the old values to the QList given as a parameter for this function.
	 *
	 * \param parameters QList containing the parameters
	 */
	void setParameters(const QList<QSharedPointer<Parameter> > &parameters);

private:

	/*!
	 * OPTIONAL
	 * Contains the parameters for the SubspaceMap instance.
	 */
	QList<QSharedPointer<Parameter> > parameters_;

};

#endif /* SUBSPACEMAP_H_ */
