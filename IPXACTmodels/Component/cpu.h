/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#ifndef CPU_H
#define CPU_H

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

    //! Cpu address space reference.
    struct AddressSpaceRef
    {
        //! The address space reference.
        QString addressSpaceRef_;

        //! The presence.
        QString isPresent_;

        /*!
         *  The constructor.
         *
         *      @param [in] addressSpaceReference   The address space reference.
         */
        IPXACTMODELS_EXPORT AddressSpaceRef(QString const& addressSpaceReference = QString());

        /*!
         *  Get the address space reference.
         *
         *      @return Address space reference.
         */
        QString getAddressSpaceRef() const;

        /*!
         *  Set the address space reference.
         *
         *      @param [in] newAddressSpaceRef  The new address space reference.
         */
        void setAddressSpaceRef(QString const& newAddressSpaceRef);

        /*!
         *  Get the is present.
         *
         *      @return Is present value.
         */
        QString getIsPresent() const;

        /*!
         *  Set the is present value.
         *
         *      @param [in] newIsPresent    The new is present value.
         */
        void setIsPresent(QString const& newIsPresent);
    };

    /*!
     *  Default constructor.
     *
     *      @param [in] name    Name of the cpu.
     */
    Cpu(QString const& name = QString());

	//! Copy constructor.
	Cpu(const Cpu &other);

	//! Assignment operator.
	Cpu &operator=(const Cpu &other);

	/*!
     *  The destructor.
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
// 	bool isValid(const QStringList& addrSpaceNames,
//         QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QStringList& errorList,
//         const QString& parentIdentifier) const;

	/*! \brief Check if the cpu is in a valid state.
	 * 
	 * \param addrSpaceNames Contains the names of address spaces the component has.
     * \param componentChoices  Choices in the containing component.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
// 	bool isValid(const QStringList& addrSpaceNames,
//         QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

	/*!
     *  Get list of the address space references for this cpu.
	 *
	 *      @return A list containing the addressSpaceRefs.
	 */
    QStringList getAddressSpaceRefs() const;

    /*!
     *  Get the address space references.
     *
     *      @return Pointer to a list of address space references.
     */
    QSharedPointer<QList<QSharedPointer<Cpu::AddressSpaceRef> > > getAddressSpaceReferences() const;

    /*!
     *  Set the address space references.
     *
     *      @param [in] newAddressSpaceRefs     Pointer to a list containing the new address space references.
     */
    void setAddressSpaceReferences(QSharedPointer<QList<QSharedPointer<AddressSpaceRef> > > newAddressSpaceRefs);

	/*!
     *  Get the parameters of the cpu.
	 *
	 *      @return Pointer to a list containing pointers to the parameters for the cpu.
	 */
	QSharedPointer<QList<QSharedPointer<Parameter> > >  getParameters();

	/*!
     *  Set the parameters for the cpu.
	 *
	 *      @param [in] parameters  Pointer to a list containing pointers to the parameters.
	 */
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters);

private:

    /*!
     *  Copy the parameters.
     *
     *      @param [in] other   The cpu being copied.
     */
    void copyParameters(const Cpu& other);

    /*!
     *  Copy the address space references.
     *
     *      @param [in] other   The cpu being copied.
     */
    void copyAddressSpaceRefs(const Cpu& other);

	//! Presence of the CPU.
	QString isPresent_;

	//! References a name of an address space within component.
    QSharedPointer<QList<QSharedPointer<AddressSpaceRef> > > addressSpaceRefs_;

	//! Specifies any cpu-type parameters.
	QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
};

#endif // CPU_H
