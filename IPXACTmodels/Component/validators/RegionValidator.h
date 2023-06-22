//-----------------------------------------------------------------------------
// File: RegionValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 21.6.2023
//
// Description:
// Validator for the ipxact:region.
//-----------------------------------------------------------------------------

#ifndef RegionVALIDATOR_H
#define RegionVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class Region;
class Segment;
class MemoryReserve;

class MemoryMapBaseValidator;
class ParameterValidator;
//-----------------------------------------------------------------------------
//! Validator for the ipxact:region.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegionValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser        The parser to use for solving expressions.
	 */
    RegionValidator(QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	~RegionValidator() = default;

    // Disable copying.
    RegionValidator(RegionValidator const& rhs) = delete;
    RegionValidator& operator=(RegionValidator const& rhs) = delete;

    /*!
     *  Validates the given address region.
     *
     *      @param [in] Region        The selected address region.
     *
     *      @return True, if the address region is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<Region> region) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The selected name.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Check if the address region contains a valid range.
     *
     *      @param [in] Region    The selected address region.
     *
     *      @return True, if the range is valid, otherwise false.
     */
    bool hasValidRange(QSharedPointer<Region> region) const;

    /*!
     *  Check if the address region contains a valid address offset.
     *
     *      @param [in] Region    The selected address region.
     *
     *      @return True, if the address offset is valid, otherwise false.
     */
    bool hasValidAddressOffset(QSharedPointer<Region> region) const;

    /*!
     *  Locate errors within an address region.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] region          The selected region.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<Region> region,
        QString const& context) const;

private:

    /*!
     *  Find errors within address region name.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] region          The selected address region.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<Region> region,
        QString const& context) const;

    /*!
     *  Find errors within address space range.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] region          The selected address region.
     *      @param [in] context         Context to help locate the error.
     */
    void findErrorsInRange(QVector<QString>& errors, QSharedPointer<Region> region,
        QString const& context) const;

    void findErrorsInOffset(QVector<QString>& errors, QSharedPointer<Region> region,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // RegionVALIDATOR_H
