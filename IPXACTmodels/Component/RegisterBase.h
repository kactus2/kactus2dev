//-----------------------------------------------------------------------------
// File: RegisterBase.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the base class for ipxact:register, ipxact:registerFile and ipxact:alternateRegister.
//-----------------------------------------------------------------------------

#ifndef REGISTERBASE_H
#define REGISTERBASE_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include <QList>
#include <QString>
#include <QSharedPointer>

class Parameter;

//-----------------------------------------------------------------------------
//! Describes the base class for ipxact:register, ipxact:registerFile and ipxact:alternateRegister.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterBase : public NameGroup, public Extendable
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] name    The register name.
     */
    RegisterBase(QString const& name = QString());

	//! The copy constructor.
    RegisterBase(const RegisterBase& other);

	//! The assignment operator.
    RegisterBase& operator=(const RegisterBase& other);

	//! The destructor.
    virtual ~RegisterBase();

	/*!
     *  Clone the register model and return pointer to the copy.
	 * 
	 *      @return Pointer to the cloned register model.
	 */
    virtual QSharedPointer<RegisterBase> clone() = 0;

	/*! \brief Check if the register model is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
    // virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    //      QStringList& errorList, const QString& parentIdentifier) const = 0;

	/*! \brief Check if the register model is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
     *
	 * \return bool True if the state is valid and writing is possible.
	*/
    // virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const = 0;

    /*!
     *  Get the presence value.
     *
     *      @return The isPresent value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the is present value.
     *
     *      @param [in] newIsPresent    The new value for is present.
     */
    void setIsPresent(QString const& newIsPresent);
    
    /*!
     *  Get the register type identifier.
     *
     *      @return The type identifier of the register.
     */
    QString getTypeIdentifier() const;

    /*!
     *  Set the register type identifier.
     *
     *      @param [in] newTypeIdentifier   The new register type identifier.
     */
    void setTypeIdentifier(QString const& newTypeIdentifier);

    /*!
     *  Get the parameters of the register.
     *
     *      @return Pointer to a list containing the parameters.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

    /*!
     *  Set the parameters for the register.
     *
     *      @param [in] newParameters   Pointer to a list containing the new parameters
     */
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters);

private:

    /*!
     *  Copy the parameters.
     *
     *      @param [in] other   The register base class being copied.
     */
    void copyParameters(const RegisterBase& other);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The presence value.
    QString isPresent_;

    //! The type identifier.
    QString typeIdentifier_;

	//! Contains the parameters.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
};

#endif // REGISTERBASE_H
