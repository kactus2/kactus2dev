/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#ifndef CPU_H_
#define CPU_H_

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include <QString>
#include <QStringList>
#include <QSharedPointer>

class Choice;
class Parameter;
class VendorExtension;

/*! \brief Equals the spirit:cpu element in IP-Xact specification
 *
 * Describes a containing component with a programmable core that has some
 * sized address space.
 */
class IPXACTMODELS_EXPORT Cpu : public NameGroup, public Extendable
{

public:
    /*!
     *  Default constructor.
     */
    Cpu();

	//! \brief Copy constructor
	Cpu(const Cpu &other);

	//! \brief Assignment operator
	Cpu &operator=(const Cpu &other);

	/*! \brief The destructor
	 *
	 */
	~Cpu();

    /*!
     *  Gets the presence.
     *
     *      @return The presence value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new presence value.
     */
    void setIsPresent(QString const& newIsPresent);

	/*! \brief Check if the cpu is in a valid state.
	 *
	 * \param addrSpaceNames Contains the names of address spaces the component has.
     * \param componentChoices  Choices in the containing component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& addrSpaceNames,
        QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
		QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the cpu is in a valid state.
	 * 
	 * \param addrSpaceNames Contains the names of address spaces the component has.
     * \param componentChoices  Choices in the containing component.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& addrSpaceNames,
        QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

	/*! \brief Get list of the address space references for this cpu
	 *
	 * \return QList containing the addressSpaceRefs
	 */
	const QStringList& getAddressSpaceRefs();

	/*! \brief Get the parameters of the cpu
	 *
	 * \return QList containing pointers to the parameters for the cpu.
	 */
	QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Get the parameters of the cpu
	 *
	 * \return QList containing pointers to the parameters for the cpu.
	 */
	const QList<QSharedPointer<Parameter> >& getParameters() const;

	/*! \brief Set the addressSpaceRefs for the cpu
	 *
	 * Calling this function will delete the old address space refs
	 *
	 * \param addressSpaceRefs QList containing the address space refs
	 */
	void setAddressSpaceRefs(const QStringList &addressSpaceRefs);

	/*! \brief Set the parameters for the cpu
	 *
	 * Calling this function will delete the old parameters.
	 *
	 * \param parameter QList containing pointers to the parameters.
	 */
	void setParameters(QList<QSharedPointer<Parameter> > &parameters);

private:

	//! Presence of the CPU.
	QString isPresent_;

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
