/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#ifndef CPU_H_
#define CPU_H_

#include <models/generaldeclarations.h>

#include <QString>
#include <QStringList>
#include <QDomNode>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class Parameter;

/*! \brief Equals the spirit:cpu element in IP-Xact specification
 *
 * Describes a containing component with a programmable core that has some
 * sized address space.
 */
class Cpu {

public:
    /*!
     *  Default constructor.
     */
    Cpu();

	/*! \brief The constructor
	 *
	 * \param cpuNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Cpu(QDomNode &cpuNode);

	//! \brief Copy constructor
	Cpu(const Cpu &other);

	//! \brief Assignment operator
	Cpu &operator=(const Cpu &other);

	/*! \brief The destructor
	 *
	 */
	~Cpu();

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

	/*! \brief Check if the cpu is in a valid state.
	 *
	 * \param addrSpaceNames Contains the names of address spaces the component has.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& addrSpaceNames,
		QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the cpu is in a valid state.
	 * 
	 * \param addrSpaceNames Contains the names of address spaces the component has.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& addrSpaceNames) const;

	/*! \brief Get list of the address space references for this cpu
	 *
	 * \return QList containing the addressSpaceRefs
	 */
	const QStringList& getAddressSpaceRefs();

	/*! \brief Get the name of this cpu
	 *
	 * \return QString containing the name of the cpu
	 */
	QString getName() const;

	/*! \brief Get the display name of the cpu.
	 *
	 * \return QString contains the display name of the cpu.
	*/
	QString getDisplayName() const;

	/*! \brief Get the description of the cpu.
	 *
	 * \return QString contains the description of the cpu.
	*/
	QString getDescription() const;

	/*! \brief Get the parameters of the cpu
	 *
	 * \return QList containing pointers to the parameters for the cpu.
	 */
	QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Set the addressSpaceRefs for the cpu
	 *
	 * Calling this function will delete the old address space refs
	 *
	 * \param addressSpaceRefs QList containing the address space refs
	 */
	void setAddressSpaceRefs(const QStringList &addressSpaceRefs);

	/*! \brief Set the name for the cpu
	 *
	 * \param name QString containig the name
	 */
	void setName(const QString &name);

	/*! \brief Set the display name for the cpu.
	 *
	 * \param displayName contains the name to set.
	 *
	*/
	void setDisplayName(const QString& displayName);

	/*! \brief Set the description for the cpu.
	 *
	 * \param description contains the description to set.
	 *
	*/
	void setDescription(const QString& description);

	/*! \brief Set the parameters for the cpu
	 *
	 * Calling this function will delete the old parameters.
	 *
	 * \param parameter QList containing pointers to the parameters.
	 */
	void setParameters(QList<QSharedPointer<Parameter> > &parameters);

private:

	//! \brief Contains the name and description.
	General::NameGroup nameGroup_;

	/*! \brief References a name of an address space within component.
	 * MANDATORY spirit:addressSpaceRef
	 */
	QStringList addressSpaceRefs_;

	/*! \brief Specifies any cpu-type parameters.
	 * OPTIONAL spirit:parameters
	 */
	QList<QSharedPointer<Parameter> > parameters_;

};

#endif /* CPU_H_ */
