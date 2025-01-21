//-----------------------------------------------------------------------------
// File: CPUValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 18.11.2015
//
// Description:
// Validator for ipxact:CPU.
//-----------------------------------------------------------------------------

#ifndef CPUVALIDATOR_H
#define CPUVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/common/validators/HierarchicalValidator.h>

#include "RegionValidator.h"

#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QVector>

class AddressSpace;
class Cpu;
class ExpressionParser;
class MemoryMap;
class ParameterValidator;

//-----------------------------------------------------------------------------
//! Validator for ipxact:CPU.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT CPUValidator : public HierarchicalValidator
{
public:

	/*!
	 *  The constructor.
	 *
     *    @param [in] parameterValidator  Validator for cpu parameters.
     *    @param [in] expressionParser    Parser for expressions in cpu.
     *    @param [in] addressSpaces       The available address spaces that may be referenced.
     *    @param [in] addressSpaces       The available memory maps that may be referenced.
     *    @param [in] revision            The applied IP-XACT standard revision.
	 */
    CPUValidator(QSharedPointer<ParameterValidator> parameterValidator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces,
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
        Document::Revision revision);

	//! The destructor.
	~CPUValidator() = default;

    // Disable copying.
    CPUValidator(CPUValidator const& rhs) = delete;
    CPUValidator& operator=(CPUValidator const& rhs) = delete;

    /*!
     *  Change the available address spaces.
     *
     *    @param [in] newAddressSpaces    The new address spaces.
     *    @param [in] addressSpaces       The available memory maps that may be referenced.
     *    @param [in] revision            The applied IP-XACT standard revision.
     */
    void componentChange(QSharedPointer<QList<QSharedPointer<AddressSpace> > > newAddressSpaces,
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
        Document::Revision revision);

    /*!
     *  Validates the given CPU.
     *
     *    @param [in] cpu           The CPU to validate.
     *
     *    @return True, if the CPU is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<Cpu> cpu) const;

    /*!
     *  Checks if the address space references of a CPU are valid.
     *
     *    @param [in] cpu   The CPU whose address space references to check.
     *
     *    @return True, if the references are valid, otherwise false.
     */
    bool hasValidAddressSpaceReferences(QSharedPointer<Cpu> cpu) const;

    /*!
     *  Checks if the memory map reference of a CPU is valid (1865-2022).
     *
     *    @param [in] cpu   The CPU whose memory map references to check.
     *
     *    @return True, if the reference is valid, otherwise false.
     */
    bool hasValidMemoryMapReference(QSharedPointer<Cpu> cpu) const;

    /*!
     *  Checks if the range of a CPU are valid (1865-2022).
     *
     *    @param [in] cpu   The CPU whose range to check.
     *
     *    @return True, if the range is valid, otherwise false.
     */
    bool hasValidRange(QSharedPointer<Cpu> cpu) const;

    /*!
     *  Checks if the width of a CPU are valid (1865-2022).
     *
     *    @param [in] cpu   The CPU whose width to check.
     *
     *    @return True, if the width is valid, otherwise false.
     */
    bool hasValidWidth(QSharedPointer<Cpu> cpu) const;

    /*!
     *  Checks if the address unit bits of a CPU are valid (1865-2022).
     *
     *    @param [in] cpu   The CPU whose address unit bits to check.
     *
     *    @return True, if the address unit bits are valid, otherwise false.
     */
    bool hasValidAddressUnitBits(QSharedPointer<Cpu> cpu) const;

    /*!
     *  Checks if the regions of a CPU are valid (1865-2022).
     *
     *    @param [in] cpu   The CPU whose regions to check.
     *
     *    @return True, if the regions are valid, otherwise false.
     */
    bool hasValidRegions(QSharedPointer<Cpu> testCpu) const;

    /*!
     *  Finds possible errors in a CPU and creates a list of them.
     *
     *    @param [in] errors      List of found errors.
     *    @param [in] cpu         The CPU whose errors to find.
     *    @param [in] context     Context to help locate the errors.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<Cpu> cpu, QString const& context) const;

    /*!
     *  Finds possible errors in the CPU regions and creates a list of them.
     *
     *    @param [in] errors      List of found errors.
     *    @param [in] cpu         The CPU whose region errors to find.
     *    @param [in] context     Context to help locate the errors.
     */   
    void findErrorsInRegions(QVector<QString>& errors, QSharedPointer<Cpu> cpu, QString const& context) const;

    /*!
     *  Get the validator used for checking regions.
     *
     *    @return The validator for regions.
     */
    QSharedPointer<RegionValidator> getRegionValidator() const;

private:

    /*!
     *  Check if the name is valid.
     *
     *    @param [in] name    The name to be evaluated.
     *
     *    @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Checks if the given reference to address space is valid.
     *
     *    @param [in] reference   The name reference for the address space.
     *
     *    @return True, if the reference is valid, otherwise false.
     */
    bool isValidAddressSpaceReference(QString const& reference) const;

    /*!
     *  Checks if the given reference to a memory map is valid.
     *
     *    @param [in] reference   The name reference for the memory map.
     *
     *    @return True, if the reference is valid, otherwise false.
     */
    bool isValidMemoryMapReference(QString const& reference) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Validator for cpu parameters.
    QSharedPointer<ParameterValidator> parameterValidator_;

    //! Validator for cpu regions.
    QSharedPointer<RegionValidator> regionValidator_;

    //! Parser for expressions in cpu.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The available address spaces for referencing.
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces_;

    //! The available memory maps for referencing.
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps_;

    //! The IP-XACT standard revision to validate against.
    Document::Revision revision_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
