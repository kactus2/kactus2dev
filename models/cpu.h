/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#ifndef CPU_H_
#define CPU_H_

#include <QString>
#include <QList>
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

	/*! \brief Get list of the address space references for this cpu
	 *
	 * \return QList containing the addressSpaceRefs
	 */
	const QList<QString>& getAddressSpaceRefs();

	/*! \brief Get the name of this cpu
	 *
	 * \return QString containing the name of the cpu
	 */
	QString getName() const;

	/*! \brief Get the parameters of the cpu
	 *
	 * \return QList containing pointers to the parameters for the cpu.
	 */
	const QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Set the addressSpaceRefs for the cpu
	 *
	 * Calling this function will delete the old address space refs
	 *
	 * \param addressSpaceRefs QList containing the address space refs
	 */
	void setAddressSpaceRefs(QList<QString> &addressSpaceRefs);

	/*! \brief Set the name for the cpu
	 *
	 * \param name QString containig the name
	 */
	void setName(const QString &name);

	/*! \brief Set the parameters for the cpu
	 *
	 * Calling this function will delete the old parameters.
	 *
	 * \param parameter QList containing pointers to the parameters.
	 */
	void setParameters(QList<QSharedPointer<Parameter> > &parameters);

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

private:

	/*! \brief Identifies the Cpu
	 * MANDATORY spirit:name
	 */
	QString name_;

	/*! \brief References a name of an address space within component.
	 * MANDATORY spirit:addressSpaceRef
	 */
	QList<QString> addressSpaceRefs_;

	/*! \brief Specifies any cpu-type parameters.
	 * OPTIONAL spirit:parameters
	 */
	QList<QSharedPointer<Parameter> > parameters_;

};

#endif /* CPU_H_ */
