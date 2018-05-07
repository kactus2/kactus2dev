//-----------------------------------------------------------------------------
// File: IndirectInterfaceValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 10.08.2017
//
// Description:
// Validator for ipxact:indirectInterface.
//-----------------------------------------------------------------------------

#ifndef INDIRECTINTERFACEVALIDATOR_H
#define INDIRECTINTERFACEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QVector>

class Component;
class Field;
class IndirectInterface;
class ExpressionParser;
class ParameterValidator;

//-----------------------------------------------------------------------------
//! Validator for ipxact:indirectInterface.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT IndirectInterfaceValidator
{
public:


    /*!
     *  The constructor.
     *
     *      @param [in] component           The component containing the indirect interface to validate.
     *      @param [in] parser              The expression parser to use in validation.
     *      @param [in] parameterValidator  The validator to use for parameters.
     */
    IndirectInterfaceValidator(QSharedPointer<Component> component,
        QSharedPointer<ExpressionParser> parser,
        QSharedPointer<ParameterValidator> parameterValidator);

	//! The destructor.
	virtual ~IndirectInterfaceValidator();
            
    void componentChange(QSharedPointer<Component> component);

    /*!
     *  Validates the given IndirectInterface.
     *
     *      @param [in] indirectInterface           The indirectInterface to validate.
     *
     *      @return True, if the IndirectInterface is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<IndirectInterface> indirectInterface) const;
    
    /*!
     *  Check if the name is valid.
     *
     *      @param [in] indirectInterface   The indirect interface to check.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<IndirectInterface> indirectInterface) const;
   
    /*!
     *  Check if the address field reference is valid.
     *
     *      @param [in] indirectInterface   The indirect interface to check.
     *
     *      @return True, if the reference is valid, otherwise false.
     */
    bool hasValidAddressReference(QSharedPointer<IndirectInterface> indirectInterface) const;
   
    /*!
     *  Check if the data field reference is valid.
     *
     *      @param [in] indirectInterface   The indirect interface to check.
     *
     *      @return True, if the reference is valid, otherwise false.
     */
    bool hasValidDataReference(QSharedPointer<IndirectInterface> indirectInterface) const;
   
    /*!
     *  Check if the memory map reference is valid.
     *
     *      @param [in] indirectInterface   The indirect interface to check.
     *
     *      @return True, if the reference is valid, otherwise false.
     */
    bool hasValidMemoryMapReference(QSharedPointer<IndirectInterface> testInterface) const;
   
    /*!
     *  Check if the transparent bridge references are valid.
     *
     *      @param [in] indirectInterface   The indirect interface to check.
     *
     *      @return True, if the references are valid, otherwise false.
     */
    bool hasValidTransparentBridges(QSharedPointer<IndirectInterface> indirectInterface) const;
       
    /*!
     *  Check if the bits in lau definition is valid.
     *
     *      @param [in] indirectInterface   The indirect interface to check.
     *
     *      @return True, if the bits in lau definition is valid, otherwise false.
     */
    bool hasValidBitsInLau(QSharedPointer<IndirectInterface> indirectInterface) const;
       
    /*!
     *  Check if the endianness is valid.
     *
     *      @param [in] indirectInterface   The indirect interface to check.
     *
     *      @return True, if the endianness is valid, otherwise false.
     */
    bool hasValidEndianness(QSharedPointer<IndirectInterface> indirectInterface) const;
       
    /*!
     *  Check if the parameters are valid.
     *
     *      @param [in] indirectInterface   The indirect interface to check.
     *
     *      @return True, if the parameters are valid, otherwise false.
     */
    bool hasValidParameters(QSharedPointer<IndirectInterface> indirectInterface) const;

    /*!
    *  Finds possible errors in an indirectInterface and creates a list of them.
    *
    *      @param [in] errors              List of found errors.
    *      @param [in] indirectInterface   The indirectInterface whose errors to find.
    *      @param [in] context             Context to help locate the errors.
    */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<IndirectInterface> indirectInterface,
        QString const& context) const;

private:

    // Disable copying.
    IndirectInterfaceValidator(IndirectInterfaceValidator const& rhs);
    IndirectInterfaceValidator& operator=(IndirectInterfaceValidator const& rhs);

    /*!
     * Finds the field with the given Id reference.
     *
     *      @param [in] fieldReference   The Id of the field to find.
     *
     *      @return The found field.
     */
    QSharedPointer<Field> findField(QString const& fieldReference) const;

    /*!
     *  Checks if the given field reference is valid.
     *
     *      @param [in] fieldReference      The Id reference to check.
     *      @param [in] targetMemoryMap     The name of the indirect memory map in the interface.
     *
     *      @returnTrue, if the reference is valid, otherwise false.
     */
    bool isValidFieldReference(QString const& fieldReference, QString const& targetMemoryMap) const;

    /*!
     *  Checks if the given field is defined within the given memory map.
     *
     *      @param [in] memoryMapName   The name of the memory map to check.
     *      @param [in] fieldReference  The Id reference of the field to check.
     *
     *      @return True, if the field is defined within the memory map, otherwise false.
     */
    bool memoryMapContainsField(QString const& memoryMapName, QString const& fieldReference) const;
    
    /*!
     *  Checks if the given indirect map has either a memory map or transparent bridges but not both.
     *
     *      @param [in] indirectInterface   The indirect interface to check.
     *
     *      @return True, if either memory map or bridges are defined, otherwise false.
     */
    bool hasEitherMemoryMapReferenceOrTransparentBridge(QSharedPointer<IndirectInterface> indirectInterface) const;
       
    /*!
    *  Finds possible errors in indirectInterface name.
    *
    *      @param [in] errors              List of found errors.
    *      @param [in] indirectInterface   The indirectInterface whose errors to find.
    *      @param [in] context             Context to help locate the errors.
    */    
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<IndirectInterface> indirectInterface, 
        QString const& context) const;
       
    /*!
    *  Finds possible errors in indirectInterface address reference.
    *
    *      @param [in] errors              List of found errors.
    *      @param [in] indirectInterface   The indirectInterface whose errors to find.
    *      @param [in] context             Context to help locate the errors.
    */    
    void findErrorsInAddressReference(QVector<QString>& errors, 
        QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const;
       
    /*!
    *  Finds possible errors in indirectInterface data reference.
    *
    *      @param [in] errors              List of found errors.
    *      @param [in] indirectInterface   The indirectInterface whose errors to find.
    *      @param [in] context             Context to help locate the errors.
    */    
    void findErrorsInDataReference(QVector<QString>& errors, QSharedPointer<IndirectInterface> indirectInterface,
        QString const& context) const;
       
    /*!
    *  Finds possible errors in an indirectInterface defining either a memory map or transparent bridges.
    *
    *      @param [in] errors              List of found errors.
    *      @param [in] indirectInterface   The indirectInterface whose errors to find.
    *      @param [in] context             Context to help locate the errors.
    */    
    void findErrorsInMemoryReference(QVector<QString>& errors, QSharedPointer<IndirectInterface> indirectInterface,
        QString const& context) const;
           
    /*!
    *  Finds possible errors in indirectInterface memory map reference.
    *
    *      @param [in] errors              List of found errors.
    *      @param [in] indirectInterface   The indirectInterface whose errors to find.
    *      @param [in] context             Context to help locate the errors.
    */    
    void findErrorsInMemoryMap(QVector<QString>& errors, QSharedPointer<IndirectInterface> indirectInterface,
        QString const& context) const;
           
    /*!
    *  Finds possible errors in indirectInterface transparent bridges.
    *
    *      @param [in] errors              List of found errors.
    *      @param [in] indirectInterface   The indirectInterface whose errors to find.
    *      @param [in] context             Context to help locate the errors.
    */   
    void findErrorsInTransparentBridges(QVector<QString>& errors,
        QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const;
               
    /*!
    *  Finds possible errors in indirectInterface bits in lau definition.
    *
    *      @param [in] errors              List of found errors.
    *      @param [in] indirectInterface   The indirectInterface whose errors to find.
    *      @param [in] context             Context to help locate the errors.
    */   
    void findErrorsInBitsInLau(QVector<QString>& errors, QSharedPointer<IndirectInterface> indirectInterface,
        QString const& context) const;
               
    /*!
    *  Finds possible errors in indirectInterface endianness.
    *
    *      @param [in] errors              List of found errors.
    *      @param [in] indirectInterface   The indirectInterface whose errors to find.
    *      @param [in] context             Context to help locate the errors.
    */   
    void findErrorsInEndianness(QVector<QString>& errors, QSharedPointer<IndirectInterface> indirectInterface,
        QString const& context) const;
               
    /*!
    *  Finds possible errors in indirectInterface parameters.
    *
    *      @param [in] errors              List of found errors.
    *      @param [in] indirectInterface   The indirectInterface whose errors to find.
    *      @param [in] context             Context to help locate the errors.
    */   
    void findErrorsInParameters(QVector<QString>& errors, QSharedPointer<IndirectInterface> indirectInterface,
        QString const& context) const;
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component containing the indirect interface to check.
    QSharedPointer<Component> component_;

    //! Parser for expressions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Validator for parameters within the indirect interfaces.
    QSharedPointer<ParameterValidator> parameterValidator_;
};

#endif // INDIRECTINTERFACEVALIDATOR_H
