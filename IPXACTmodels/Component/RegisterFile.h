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

class MemoryArray;

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

  QStringList getAllRegisterOffsets() const;
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

    /*!
     *	Get the register file definition reference.
     *
     * 	    @return The reference to a register file definition.
     */
    QString getRegisterFileDefinitionReference() const;

    /*!
     *	Set the register file definition reference.
     *
     *      @param [in] registerFileDefinitionRef     The new register file definition reference.
     */
    void setRegisterFileDefinitionReference(QString const& newRegisterFileDefinitionRef);

    /*!
     *	Get the reference to the type definitions containing the register file definition reference.
     *
     * 	    @return The reference to the type definitions containing the register file definition.
     */
    QString getTypeDefinitionsReference() const;

    /*!
     *	Set the type definitions reference.
     *
     *      @param [in] typeDefinitionsRef     The new type definitions reference to set.
     */
    void setTypeDefinitionsReference(QString const& newTypeDefinitionsRef);

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

	//! The range of a registerFile.
    QString range_;

	//! Contains pointers to the register data.
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData_;

    //! The name of the referenced register file definition.
    QString registerFileDefinitionReference_;

    //! The type definitions containing the referenced register file definition.
    QString typeDefinitionsReference_;

};

Q_DECLARE_METATYPE(QSharedPointer<RegisterFile>);

#endif // REGISTERFILE_H
