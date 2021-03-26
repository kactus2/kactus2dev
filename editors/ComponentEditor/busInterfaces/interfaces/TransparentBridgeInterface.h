//-----------------------------------------------------------------------------
// File: TransparentBridgeInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2021
//
// Description:
// Interface for accessing transparent bridges.
//-----------------------------------------------------------------------------

#ifndef TRANSPARENTBRIDGEINTERFACE_H
#define TRANSPARENTBRIDGEINTERFACE_H

#include <editors/ComponentEditor/common/interfaces/ParameterizableInterface.h>
#include <editors/ComponentEditor/common/interfaces/CommonInterface.h>

#include <QSharedPointer>

class TransparentBridge;
class BusInterfaceValidator;
class IndirectInterfaceValidator;

//-----------------------------------------------------------------------------
//! Interface for accessing transparent bridges.
//-----------------------------------------------------------------------------
class TransparentBridgeInterface : public ParameterizableInterface, public CommonInterface
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] expressionParser            Parser for expressions.
     *      @param [in] expressionFormatter         Formatter for expressions.
     */
    TransparentBridgeInterface(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter);

    /*!
     *  The destructor.
     */
    virtual ~TransparentBridgeInterface() = default;

    /*!
     *  Setup the transparent bridges.
     *
     *      @param [in] newBridges  The new transparent bridges.
     */
    void setBridges(QSharedPointer<QList<QSharedPointer<TransparentBridge> > > newBridges);

    /*!
     *  Get index of the transparent bridge containing the selected master bus interface reference.
     *
     *      @param [in] masterReference     The selected master bus interface reference.
     *
     *      @return Index of the selected transparent bridge.
     */
    int getItemIndex(std::string const& masterReference) const;

    /*!
     *  Get master bus interface reference of the indexed transparent bridge.
     *
     *      @param [in] itemIndex   Index of the selected transparent bridge.
     *
     *      @return Master bus interface reference of the selected transparent bridge.
     */
    std::string getIndexedMasterReference(int const& itemIndex) const;

    /*!
     *  Get the number of available transparent bridges.
     *
     *      @return Number of available transparent bridges.
     */
    virtual int itemCount() const override final;

    /*!
     *  Get the master bus interface references of the available transparent bridges.
     *
     *      @return Master bus interface references of the available ports abstractions.
     */
    virtual std::vector<std::string> getItemNames() const override final;

    /*!
     *  Set a new master bus interface reference for the selected transparent bridge.
     *
     *      @param [in] currentMaster   Master bus interface reference of the selected transparent bridge.
     *      @param [in] newMaster       New master bus interface reference for the selected transparent bridge.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setMasterReference(std::string const& currentMaster, std::string const& newMaster);

    /*!
     *  Validates the contained transparent bridges.
     *
     *      @return True, if all the transparent bridges are valid, false otherwise.
     */
    virtual bool validateItems() const override final;

    /*!
     *  Get the calculated is present value of the selected transparent bridge.
     *
     *      @param [in] masterReference     Master bus interface reference of the selected transparent bridge.
     *      @param [in] baseNumber          Base for displaying the value.
     *
     *      @return Calculated is present value of the selected transparent bridge.
     */
    std::string getIsPresentValue(std::string const& masterReference, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted is present expression of the selected transparent bridge.
     *
     *      @param [in] masterReference     Master bus interface reference of the selected transparent bridge.
     *
     *      @return Formatted is present expression of the selected transparent bridge.
     */
    std::string getIsPresentFormattedExpression(std::string const& masterReference) const;

    /*!
     *  Get the is present expression of the selected transparent bridge.
     *
     *      @param [in] masterReference     Master bus interface reference of the selected transparent bridge.
     *
     *      @return Is present expression of the selected transparent bridge.
     */
    std::string getIsPresentExpression(std::string const& masterReference) const;

    /*!
     *  Set the is present expression for the selected transparent bridge.
     *
     *      @param [in] masterReference     Master bus interface reference of the selected transparent bridge.
     *      @param [in] newIsPresent        New is present expression.
     *
     *      @return True, if successful, false otherwise.
     */
    bool setIsPresent(std::string const& masterReference, std::string const& newIsPresent);
    
    /*!
     *  Add a new transparent bridge.
     *
     *      @param [in] bridgeIndex     Index of the new transparent bridge.
     *      @param [in] bridgeName      Master bus interface reference of the new transparent bridge.
     *
     *      @return True, if successful, false otherwise.
     */
    bool addBridge(int const& bridgeIndex, std::string const& bridgeName = "") const;

    /*!
     *  Remove the selected transparent bridge.
     *
     *      @param [in] masterReference     Master bus interface reference of the selected transparent bridge.
     *
     *      @return True, if successful, false otherwise.
     */
    bool removeTransparentBridge(std::string const& masterReference);

    /*!
     *  Get all the references made to the selected ID in the selected transparent bridge.
     *
     *      @param [in] itemName    Master bus interface reference of the selected transparent bridge.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references made to the selected ID.
     */
    virtual int getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const;

    /*!
     *  Get all the expressions from the selected transparent bridge.
     *
     *      @param [in] masterReference     Master reference of the selected transparent bridge.
     *
     *      @return List of expressions in the selected transparent bridge.
     */
    std::vector<std::string> getAllExpressions(std::string const& masterReference) const;

    //! No copying. No assignment.
    TransparentBridgeInterface(const TransparentBridgeInterface& other) = delete;
    TransparentBridgeInterface& operator=(const TransparentBridgeInterface& other) = delete;

private:

    /*!
     *  Get the selected transparent bridge.
     *
     *      @param [in] masterReference     Master bus interface reference of the selected transparent bridge.
     *
     *      @return The selected transparent bridge.
     */
    QSharedPointer<TransparentBridge> getTransparentBridge(std::string const& masterReference) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of available transparent bridges.
    QSharedPointer<QList<QSharedPointer<TransparentBridge> > > bridges_;

    //! Validator for bus interfaces.
    QSharedPointer<BusInterfaceValidator> busValidator_;

    //! Validator for indirect interfaces.
    QSharedPointer<IndirectInterfaceValidator> indirectValidator_;
};

#endif // TRANSPARENTBRIDGEINTERFACE_H
