//-----------------------------------------------------------------------------
// File: AddressSpaceValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.11.2015
//
// Description:
// Validator for the ipxact:addressSpace.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEVALIDATOR_H
#define ADDRESSSPACEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class AddressSpace;
class Segment;
class MemoryReserve;

class MemoryMapBaseValidator;
class ParameterValidator2014;
//-----------------------------------------------------------------------------
//! Validator for the ipxact:addressSpace.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AddressSpaceValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser        The parser to use for solving expressions.
     *      @param [in] memoryMapBaseValidator  Validator used for local memory map.
     *      @param [in] parameterValidator      Validator used for parameters.
	 */
    AddressSpaceValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<MemoryMapBaseValidator> memoryMapBaseValidator,
        QSharedPointer<ParameterValidator2014> parameterValidator);

	//! The destructor.
	virtual ~AddressSpaceValidator();
    
    /*!
     *  Get the validator used for local memory map.
     *
     *      @return The validator used for local memory map.
     */
    QSharedPointer<MemoryMapBaseValidator> getLocalMemoryMapValidator();

    /*!
     *  Validates the given address space.
     *
     *      @param [in] addressSpace        The selected address space.
     *
     *      @return True, if the address space is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<AddressSpace> addressSpace) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The selected name.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Check if the is present value is valid.
     *
     *      @param [in] isPresent   The selected is present value.
     *
     *      @return True, if the is present is valid, otherwise false.
     */
    bool hasValidIsPresent(QString const& isPresent) const;

    /*!
     *  Check if the address space contains a valid range.
     *
     *      @param [in] addressSpace    The selected address space.
     *
     *      @return True, if the range is valid, otherwise false.
     */
    bool hasValidRange(QSharedPointer<AddressSpace> addressSpace) const;

    /*!
     *  Check if the address space contains a valid width.
     *
     *      @param [in] addressSpace    The selected address space.
     *
     *      @return True, if the width is valid, otherwise false.
     */
    bool hasValidWidth(QSharedPointer<AddressSpace> addressSpace) const;

    /*!
     *  Check if the address space contains valid segments.
     *
     *      @param [in] addressSpace    The selected address space.
     *
     *      @return True, if the segments are valid, otherwise false.
     */
    bool hasValidSegments(QSharedPointer<AddressSpace> addressSpace) const;

    /*!
     *  Check if the address space contains a valid address unit bits value.
     *
     *      @param [in] addressSpace    The selected address space.
     *
     *      @return True, if the address unit bits value is valid, otherwise false.
     */
    bool hasValidAddressUnitBits(QSharedPointer<AddressSpace> addressSpace) const;

    /*!
     *  Check if the address space contains a valid local memory map.
     *
     *      @param [in] addressSpace    The selected address space.
     *
     *      @return True, if the local memory map is valid, otherwise false.
     */
    bool hasValidLocalMemoryMap(QSharedPointer<AddressSpace> addressSpace) const;

    /*!
     *  Check if the address space contains valid parameters.
     *
     *      @param [in] addressSpace    The selected address space.
     *
     *      @return True, if the parameters are valid, otherwise false.
     */
    bool hasValidParameters(QSharedPointer<AddressSpace> addressSpace) const;

    /*!
     *  Locate errors within an address space.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressSpace    The selected address space.
     *      @param [in] context         Context to help locate the error.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
        QString const& context) const;

private:

	// Disable copying.
	AddressSpaceValidator(AddressSpaceValidator const& rhs);
	AddressSpaceValidator& operator=(AddressSpaceValidator const& rhs);

    /*!
     *  Check if the segment contains a valid address offset.
     *
     *      @param [in] segment     The selected segment.
     *
     *      @return True, if the address offset is valid, otherwise false.
     */
    bool segmentHasValidAddressOffset(QSharedPointer<Segment> segment) const;

    /*!
     *  Check if the segment contains a valid range.
     *
     *      @param [in] segment     The selected segment.
     *
     *      @return True, if the range is valid, otherwise false.
     */
    bool segmentHasValidRange(QSharedPointer<Segment> segment) const;

    /*!
     *  Find errors within address space name.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressSpace    The selected address space.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
        QString const& context) const;

    /*!
     *  Find errors within address space isPresent.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressSpace    The selected address space.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
        QString const& context) const;

    /*!
     *  Find errors within address space range.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressSpace    The selected address space.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInRange(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
        QString const& context) const;

    /*!
     *  Find errors within address space width.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressSpace    The selected address space.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInWidth(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
        QString const& context) const;

    /*!
     *  Find errors within address space segments.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressSpace    The selected address space.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInSegments(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
        QString const& context) const;

    /*!
     *  Find errors within address space address unit bits.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressSpace    The selected address space.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInAddressUnitBits(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
        QString const& context) const;

    /*!
     *  Find errors within address space local memory map.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressSpace    The selected address space.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInLocalMemoryMap(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
        QString const& context) const;

    /*!
     *  Find errors within address space parameters.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] addressSpace    The selected address space.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInParameters(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The validator used for local memory map.
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator_;

    //! The validator used for parameters.
    QSharedPointer<ParameterValidator2014> parameterValidator_;
};

#endif // ADDRESSSPACEVALIDATOR_H
