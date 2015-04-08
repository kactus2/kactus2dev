/* 
 *
 *  Created on: 30.7.2010
 *      Author: Antti Kamppi
 */

#ifndef REMAPSTATE_H_
#define REMAPSTATE_H_

#include "remapport.h"

#include "ipxactmodels_global.h"

#include <IPXACTmodels/NameGroup.h>

#include <QString>
#include <QList>
#include <QXmlStreamWriter>

/*! \brief Equals to the spirit:remapState element in the IP-Xact specification
 *
 * Defines a conditional remap state where each state is conditioned by a remap
 * port specified with a remapPort instance.
 */
class IPXACTMODELS_EXPORT RemapState {

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] remapNode   A reference to the QDomNode to parse the information from.
     *
     *      Exception guarantee: basic
     *      Parse_error Occurs when a mandatory element is missing in this class or one of it's member classes.
	 */
	RemapState(QDomNode &remapNode);

    /*!
     *  The default constructor.
     */
    RemapState();

	/*!
	 *  Copy constructor.
	 *
	 *      @param [in] other   The remap state to be copied.
	 */
	RemapState(const RemapState &other);

    /*!
	 *  Assignment operator.
	 *
	 *      @param [in] other   The remap state being assigned.
	 */
	RemapState &operator=(const RemapState &other);

	/*!
	 *  The destructor.
	 */
	~RemapState();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/

	/*!
	 *  Write the contents of the class as a valid IP-Xact using the writer.
	 *
	 *      @param [in] writer  A reference to a QXMLStreamWriter instance that is used to write the document into
     *                          a file.
	 */
	void write(QXmlStreamWriter& writer);

    /*!
	 *  Check if the remap state is in a valid state.
	 *
	 *      @param [in] portNames           List containing the names of the ports of the component.
	 *      @param [in] errorList           The list to add the possible error messages to.
	 *      @param [in] parentIdentifier    String from parent to help identify the location of the erro.
     *
     *      @return True, if the state is valid, false otherwise.
	 */
	bool isValid(const QStringList& portNames, QStringList& errorList, const QString& parentIdentifier) const;

	/*!
	 *  Check if the remap state is in a valid state.
	 *
	 *      @param [in] portNames   List containing the names of the ports of the component.
     *
     *      @return True, if the state is valid and writing is possible.
	 */
	bool isValid(const QStringList& portNames) const;

    /*!
	 *  Get the name of the remap state.
	 *
     *      @return The name of the remap state.
	 */
	QString getName() const;

    /*!
	 *  Get the list of the remap ports for this remap state.
	 *
     *      @return List containing pointer to the remap port instances.
	 */
	const QList<QSharedPointer<RemapPort> >& getRemapPorts();

    /*!
	 *  Set the name for this remap state.
	 *
	 *      @param [in] name    The new name of the remap state.
	 */
	void setName(const QString &name);

    /*!
	 *  Set the remap ports for this remap state.
	 *
	 *      @param [in] remapPorts  A list containing pointers to the remap ports.
	 */
	void setRemapPorts(const QList<QSharedPointer<RemapPort> > &remapPorts);

    /*!
     *  Get the display name of the remap state.
     *
     *      @return The display name for the remap state.
     */
    QString getDisplayName() const;

    /*!
     *  Set the display name for the remap state.
     *
     *      @param [in] newDisplayName  The new display name for the remap state.
     */
    void setDisplayName(QString const& newDisplayName);

    /*!
     *  Get the description of the remap state.
     *
     *      @return The description of the remap state.
     */
    QString getDescription() const;

    /*!
     *  Set the description for the remap state.
     *
     *      @param [in] newDescription  The new description for the remap state.
     */
    void setDescription(QString const& newDescription);

    /*!
     *  Get the name group of the remap state.
     *
     *      @return The name group of the remap state.
     */
    NameGroup& getNamegroup();

    /*!
     *  Get the name group of the remap state.
     *
     *      @return The name group of the remap state.
     */
    const NameGroup& getNamegroup() const;

private:

	/*!
	 * MANDATORY
	 * Identifies the remapState
	 */
	//QString name_;
    NameGroup nameGroup_;

	/*!
	 * OPTIONAL
	 * List of the remapPorts for this remap state.
	 */
	QList<QSharedPointer<RemapPort> > remapPorts_;
};

#endif /* REMAPSTATE_H_ */
