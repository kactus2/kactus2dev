/* 
 *
 *  Created on: 31.8.2010
 *      Author: Antti Kamppi
 */

#ifndef SLAVEINTERFACE_H_
#define SLAVEINTERFACE_H_

#include "ipxactmodels_global.h"

#include <QString>
#include <QList>
#include <QStringList>
#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>

/*! \brief Equals the spirit:slave element in IP-Xact specification.
 *
 * Slave interface is one that responds to transactions.
 */
class IPXACTMODELS_EXPORT SlaveInterface {

public:

	/*! \brief Equals the spirit:bridge element in IP-Xact specification
	 *
	 * Contains a reference to a master interface.
	 */
	struct Bridge {

		/*! \brief MANDATORY spirit:masterRef
		 * Reference to a master interface.
		 */
		QString masterRef_;

		/*! \brief MANDATORY spirit:opaque
		 * Defines the type of bridging.
		 */
		bool opaque_;

		/*! \brief The constructor
		 *
		 * \param bridgeNode A reference to a QDomNode to parse the
		 * information from.
		 *
		 * Exception guarantee: basic
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this class or one of it's member classes.
		 */
		Bridge(QDomNode& bridgeNode);

		/*! \brief The default constructor.
		 *
		*/
		IPXACTMODELS_EXPORT  Bridge();
	};

	/*! \brief Equals the spirit:fileSetRefGroup in IP-Xact specifitation.
	 *
	 * A reference to a fileset contained in this component that is associated
	 * with this slave interface.
	 */
	struct FileSetRefGroup {

		/*! \brief OPTIONAL spirit:group
		 * Allows the definition of a group name for the fileSetRefGroup
		 */
		QString group_;

		/*! \brief OPTIONAL spirit:fileSetRef
		 * An unbounded list of references to a fileSet by name.
		 */
		QStringList fileSetRefs_;

		/*! \brief The constructor
		 *
		 * \param fileSetNode A reference to a QDomNode to parse the
		 * information from.
		 *
		 * Exception guarantee: basic
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this class or one of it's member classes.
		 */
		FileSetRefGroup(QDomNode& fileSetNode);
	};

	/*! \brief The constructor
	 *
	 * \param slaveNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	SlaveInterface(QDomNode& slaveNode);

	/*! \brief The default constructor
	 *
	*/
	SlaveInterface();

	//! \brief Copy constructor
	SlaveInterface(const SlaveInterface& other);

	//! \brief Assignment operator
	SlaveInterface& operator=(const SlaveInterface& other);
	
	/*! \brief The destructor
	 *
	 */
	~SlaveInterface();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the slave is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the slave is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

	/*! \brief Get list of the bridges for this component
	 *
	 * \return A reference to a QList containing pointers to the bridge
	 * instances.
	 */
	const QList<QSharedPointer<Bridge> >& getBridges() const;

	/*! \brief Get list of the bridges for this component
	 *
	 * \return A reference to a QList containing pointers to the bridge
	 * instances.
	 */
	QList<QSharedPointer<Bridge> >& getBridges();

	/*! \brief Get list of the master interfaces referred to through the contained bridge-elements.
	 *
	 * Method: 		getMasterReferences
	 * Full name:	SlaveInterface::getMasterReferences
	 * Access:		public 
	 *
	 *
	 * \return QStringList containing the names of the referred master interfaces.
	*/
	QStringList getMasterReferences() const;

	/*! \brief Check if the slave is connected to a master interface through bridge.
	 *
	 * Method: 		hasBridge
	 * Full name:	SlaveInterface::hasBridge
	 * Access:		public 
	 *
	 *
	 * \return True if at least one bridge is found.
	*/
	bool hasBridge() const;

	/*! \brief Get list of the fileSetRefGroups for this component
	 *
	 * \return A reference to a QList containing pointers to the
	 * fileSetRefGroup instances.
	 */
	const QList<QSharedPointer<FileSetRefGroup> >& getFileSetRefGroup();

	/*!  \brief Get the name of the memoryMap
	 *
	 * \return QString containing the name of the memory map defined in the
	 * containing description.
	 */
	QString getMemoryMapRef() const;

	/*! \brief Set the bridges for this interface
	 *
	 * Calling this function will delete old bridge instances.
	 *
	 * \param bridges A reference to a QList containing pointers to the
	 * new bridge instances.
	 */
	void setBridges(const QList<QSharedPointer<Bridge> >& bridges);

	/*! \brief Set the references to the file sets
	 *
	 * Calling this function will delete old FileSetRefGroup instances.
	 *
	 * \param fileSetRefGroup A reference to a QList containing pointers to
	 * the new FileSetRefGroup instances.
	 */
	void setFileSetRefGroup(const
			QList<QSharedPointer<FileSetRefGroup> >& fileSetRefGroup);

	/*! \brief Set the memory map for this interface
	 *
	 * \param memoryMapRef A reference to a QString containing the name of
	 * the memory map.
	 */
	void setMemoryMapRef(const QString& memoryMapRef);

private:

	/*! \brief OPTIONAL spirit:memoryMapRef
	 * Contains an attribute that references a memory map.
	 */
	QString memoryMapRef_;

	/*! \brief OPTIONAL spirit:bridge
	 * An unbounded list of references to master interface.
	 */
	QList<QSharedPointer<Bridge> > bridges_;

	/*! \brief OPTIONAL spirit:fileSetRefGroup
	 * A pointer to a fileSetRefGroup instance.
	 */
	QList<QSharedPointer<FileSetRefGroup> > fileSetRefGroup_;
};

#endif /* SLAVEINTERFACE_H_ */
