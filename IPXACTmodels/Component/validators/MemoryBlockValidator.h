//-----------------------------------------------------------------------------
// File: MemoryBlockValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2022
//
// Description:
// Validator for memory block.
//-----------------------------------------------------------------------------

#ifndef MEMORYBLOCKVALIDATOR_H
#define MEMORYBLOCKVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class MemoryBlockBase;
class ParameterValidator;

//-----------------------------------------------------------------------------
//! Validator for memory block.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryBlockValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser    The parser to use for solving expressions.
     *      @param [in] parameterValidator  Validator used for parameters.
	 */
    MemoryBlockValidator(QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterValidator> parameterValidator, Document::Revision docRevision);

	/*!
     *  The destructor.
     */
    ~MemoryBlockValidator() = default;

    /*!
     *  Validates the given memory block.
     *
     *      @param [in] memoryBlock     The memory block to validate.
     *
     *      @return True, if the memory block is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<MemoryBlockBase> memoryBlock) const;

    /*!
     *  Check if the memory block contains a valid name.
     *
     *      @param [in] memoryBlock     The selected memory block.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<MemoryBlockBase> memoryBlock) const;

    /*!
     *  Check if the memory block contains a valid isPresent value.
     *
     *      @param [in] memoryBlock     The selected memory block.
     *
     *      @return True, if the isPresent is valid, otherwise false.
     */
    bool hasValidIsPresent(QSharedPointer<MemoryBlockBase> memoryBlock) const;

    /*!
     *  Check if the memory block contains a valid base address.
     *
     *      @param [in] memoryBlock     The selected memory block.
     *
     *      @return True, if the base address is valid, otherwise false.
     */
    bool hasValidBaseAddress(QSharedPointer<MemoryBlockBase> memoryBlock) const;

    /*!
     *  Check if the memory block contains valid parameters.
     *
     *      @param [in] memoryBlock     The selected memory block.
     *
     *      @return True, if the parameters are valid, otherwise false.
     */
    bool hasValidParameters(QSharedPointer<MemoryBlockBase> memoryBlock) const;

protected:

    /*!
     *  Get the expression parser.
     *
     *      @return The parser for expressions.
     */
    QSharedPointer<ExpressionParser> getExpressionParser() const;
    
    /*!
     *  Find errors within a name.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] memoryBlock     The selected memory block.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<MemoryBlockBase> memoryBlock,
        QString const& context) const;

    /*!
     *  Find errors within isPresent.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] memoryBlock     The selected memory block.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<MemoryBlockBase> memoryBlock,
        QString const& context) const;

    /*!
     *  Find errors within base address.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] memoryBlock     The selected memory block.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInBaseAddress(QVector<QString>& errors, QSharedPointer<MemoryBlockBase> memoryBlock,
        QString const& context) const;

    /*!
     *  Find errors within memory block parameters.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] memoryBlock     The selected memory block.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInParameters(QVector<QString>& errors, QSharedPointer<MemoryBlockBase> memoryBlock,
        QString const& context) const;

    //! The IP-XACT standard revision to comply to.
    Document::Revision docRevision_;

private:

	// Disable copying.
	MemoryBlockValidator(MemoryBlockValidator const& rhs);
    MemoryBlockValidator& operator=(MemoryBlockValidator const& rhs);

    /*!
     *  Get the type of the memory block.
     *
     *      @return Identification type of the memory block.
     */
    virtual QString getBlockType() const = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The validator used for parameters.
    QSharedPointer<ParameterValidator> parameterValidator_;
};

#endif // MEMORYBLOCKVALIDATOR_H
