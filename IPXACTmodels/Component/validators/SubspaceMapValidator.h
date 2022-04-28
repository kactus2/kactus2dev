//-----------------------------------------------------------------------------
// File: SubspaceMapValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2022
//
// Description:
// Validator for ipxact:subspaceMap.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAPVALIDATOR_H
#define SUBSPACEMAPVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/validators/MemoryBlockValidator.h>

#include <QSharedPointer>
#include <QString>

class SubSpaceMap;
class Component;
class BusInterface;
class AddressSpace;

//-----------------------------------------------------------------------------
//! Validator for ipxact:subspaceMap.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SubspaceMapValidator : public MemoryBlockValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser    The parser to use for solving expressions.
     *      @param [in] parameterValidator  Validator used for parameters.
	 */
    SubspaceMapValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterValidator> parameterValidator);

	/*!
     *  The destructor.
     */
    virtual ~SubspaceMapValidator() = default;
    
    /*!
     *  Change the containing component.
     *
     *      @param [in] component   The new component.
     */
    void componentChange(QSharedPointer<Component> component);

    /*!
     *  Validates the given subspace map.
     *
     *      @param [in] subspace    The subspace map to validate.
     *
     *      @return True, if the subspace map is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<SubSpaceMap> subspace) const;

    /*!
     *  Check if the selected subspace map has a valid master bus interface reference.
     *
     *      @param [in] subspace    The selected subspace map.
     *
     *      @return True, if the master bus interface reference is valid, false otherwise.
     */
    bool hasValidMasterReference(QSharedPointer<SubSpaceMap> subspace) const;

    /*!
     *  Check if the selected subspace map has a valid segment reference.
     *
     *      @param [in] subspace    The selected subspace map.
     *
     *      @return True, if the segment reference is valid, false otherwise.
     */
    bool hasValidSegmentReference(QSharedPointer<SubSpaceMap> subspace) const;

    /*!
     *  Locate errors within a subspace map.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] subspace    The selected subspace map.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<SubSpaceMap> subspace, QString const& context)
        const;

private:

	// Disable copying.
	SubspaceMapValidator(SubspaceMapValidator const& rhs);
    SubspaceMapValidator& operator=(SubspaceMapValidator const& rhs);

    /*!
     *  Get the type of the memory block.
     *
     *      @return Identification type of the memory block.
     */
    virtual QString getBlockType() const override final;

    /*!
     *  Check if the referenced master bus interface exists.
     *
     *      @param [in] masterReference     Reference to the selected master bus interface.
     *
     *      @return True, if the master bus interface reference exists, false otherwise.
     */
    bool masterReferenceExists(QString const& masterReference) const;

    /*!
     *  Check if the referenced bus interface is master.
     *
     *      @param [in] masterReference     Reference to the selected bus interface.
     *
     *      @return True, if the bus interface is master, false otherwise.
     */
    bool referencedBusIsMaster(QString const& masterReference) const;

    /*!
     *  Check if the address space within the selected master bus interface contains the selected segment.
     *
     *      @param [in] masterReference     Reference to the selected master bus interface.
     *      @param [in] segmentReference    Reference to the selected segment.
     *
     *      @return True, if the address space contains the selected segment, false otherwise.
     */
    bool spaceContainsSegmentReference(QString const& masterReference, QString const& segmentReference) const;

    /*!
     *  Get the selected bus interface.
     *
     *      @param [in] masterReference     Reference to the selected bus interface.
     *
     *      @return The selected bus interface.
     */
    QSharedPointer<BusInterface> getReferencedBusInterface(QString const& masterReference) const;

    /*!
     *  Get the address space within the selected master bus interface.
     *
     *      @param [in] masterReference     Reference to the selected bus interface.
     *
     *      @return Address space within the selected master bus interface.
     */
    QSharedPointer<AddressSpace> getReferencedSpace(QString const& masterReference) const;

    /*!
     *  Find errors within master bus interface reference.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] subspace    The selected subspace map.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInMasterReference(QVector<QString>& errors, QSharedPointer<SubSpaceMap> subspace,
        QString const& context) const;

    /*!
     *  Find errors within segment reference.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] subspace    The selected subspace map.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInSegmentReference(QVector<QString>& errors, QSharedPointer<SubSpaceMap> subspace,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of available bus interfaces.
    QSharedPointer<QList<QSharedPointer<BusInterface> > > availableBuses_;

    //! List of available address spaces.
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > availableSpaces_;
};

#endif // SUBSPACEMAPVALIDATOR_H
