//-----------------------------------------------------------------------------
// File: RegisterFile.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the ipxact:registerFile element.
//-----------------------------------------------------------------------------

#ifndef REGISTERFILE_H
#define REGISTERFILE_H

#include "RegisterBase.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Describes the ipxact:registerFile element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterFile: public RegisterBase
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] name            The register file name.
     *      @param [in] addressOffset   The address offset.
     *      @param [in] range           The range of the register file.
     */
    RegisterFile(QString const& name = QString(), QString const& addressOffset = QString(),
        QString const& range = QString());

	//! Copy constructor.
	RegisterFile(const RegisterFile& other);

	//! Assignment operator.
	RegisterFile& operator=(const RegisterFile& other);

	//! The destructor.
	virtual ~RegisterFile();

	/*!
     *  Clone the register file and return pointer to the copy.
	 * 
	 *      @return Pointer to the cloned register file.
	 */
    virtual QSharedPointer<RegisterBase> clone();

	/*! \brief Check if the register file is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
    // virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    //      QStringList& errorList, const QString& parentIdentifier) const;

	/*! \brief Check if the register file is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
     *
	 * \return bool True if the state is valid and writing is possible.
	*/
    // virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

	/*!
     *  Get the dimension of the register.
	 *
	 *      @return The value of the dim-element.
	 */
    QString getDimension() const;

	/*!
     *  Set the dimension of the registerFile.
	 *
	 *      @param [in] newDimension    The new value for dim-element.
	 */
    void setDimension(QString const& newDimension);

	/*!
     *  Get the address offset of the registerFile.
	 *
	 *      @return The offset.
	 */
	QString getAddressOffset() const;
    
	/*!
     *  Set the address offset.
	 *
	 *      @param [in] newAddressOffset    The new offset.
	 */
	void setAddressOffset(QString const& newAddressOffset);

	/*!
     *  Get the range of the registerFile.
	 *
	 *      @return The range of the registerFile.
	 */
    QString getRange() const;
    
	/*!
     *  Set the range of the registerFile.
	 *
	 *      @param [in] newRange    The new range.
	 */
    void setRange(QString const& newRange);

	/*!
     *  Get the registerData elements of the reigsterFile.
	 *
	 *      @return Pointer to a list containing the register data.
	 */
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > getRegisterData() const;
    
	/*!
     *  Set the registerData for the registerFile.
	 *
	 *      @param [in] newRegisterData     Pointer to a list containing the register data.
	 */
    void setRegisterData(QSharedPointer<QList<QSharedPointer<RegisterBase> > > newRegisterData);

private:

    /*!
     *  Copy the register data.
     *
     *      @param [in] other   The register file being copied.
     */
    void copyRegisterData(const RegisterFile& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Contains dimensions of a register array.
    QString dimension_;

	//! Offset from the base address.
	QString addressOffset_;

	//! The range of a registerFile.
    QString range_;

	//! Contains pointers to the register data.
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData_;
};

#endif // REGISTERFILE_H
