//-----------------------------------------------------------------------------
// File: ParameterizableInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 13.03.2020
//
// Description:
// Interface for parameterizable items.
//-----------------------------------------------------------------------------

#ifndef PARAMETERIZABLEINTERFACE_H
#define PARAMETERIZABLEINTERFACE_H

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

class ExpressionFormatter;
class ValueFormatter;

using namespace std;

//-----------------------------------------------------------------------------
//! Interface for parameterizable items.
//-----------------------------------------------------------------------------
class ParameterizableInterface
{
public:

    /*!
     *  The constructor.
     */
    ParameterizableInterface();

	/*!
     *  The destructor.
     */
    virtual ~ParameterizableInterface() = default;

    /*!
     *  Set expression parser.
     *
     *      @param [in] parser  Parser for expressions.
     */
    void setExpressionParser(QSharedPointer<ExpressionParser> parser);

    /*!
     *  Set expression formatter.
     *
     *      @param [in] formatter   Formatter for expressions.
     */
    void setExpressionFormatter(QSharedPointer<ExpressionFormatter> formatter);

    /*!
     *  Get index of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Index of the selected item.
     */
    virtual int getItemIndex(string const& itemName) const = 0;

    /*!
     *  Get name of the indexed item.
     *
     *      @param [in] itemIndex   Index of the selected item.
     *
     *      @return Name of the selected item.
     */
    virtual string getIndexedItemName(int const& itemIndex) const = 0;

    /*!
     *  Get the number of available items.
     *
     *      @return Number of available items.
     */
    virtual int itemCount() const = 0;

    /*!
     *  Get the names of the available items.
     *
     *      @return Names of the available items.
     */
    virtual vector<string> getItemNames() const = 0;

    /*!
     *  Set a new name for the selected item.
     *
     *      @param [in] currentName     Name of the selected item.
     *      @param [in] newName         New name for the item.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setName(string const& currentName, string const& newName) = 0;

    /*!
     *  Get the description of the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return Description of the selected item.
     */
    virtual string getDescription(string const& itemName) const = 0;

    /*!
     *  Set a new description for the selected item.
     *
     *      @param [in] itemName        Name of the selected item.
     *      @param [in] newDescription  New description.
     *
     *      @return True, if successful, false otherwise.
     */
    virtual bool setDescription(string const& itemName, string const& newDescription) = 0;

    /*!
     *  Calculate all the references to the selected ID in the selected item.
     *
     *      @param [in] itemName    Name of the selected item.
     *      @param [in] valueID     The selected ID.
     *
     *      @return Number of references to the selected ID in the selected item.
     */
    virtual int getAllReferencesToIdInItem(const string& itemName, string const&  valueID) const = 0;

    /*!
     *  Validates the contained items.
     *
     *      @return True, if all the ports are valid, false otherwise.
     */
    virtual bool validateItems() const = 0;

    /*!
     *  Check if the selected item has a valid name.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return True, if the name is valid, false otherwise.
     */
    virtual bool itemHasValidName(string const& itemName) const = 0;

    //! No copying. No assignment.
    ParameterizableInterface(const ParameterizableInterface& other) = delete;
    ParameterizableInterface& operator=(const ParameterizableInterface& other) = delete;

protected:

    /*!
     *  Get the formatted value for the given expression.
     *
     *      @param [in] expression  The selected expression.
     *
     *      @return The formatted expression.
     */
    QString formattedValueFor(QString const& expression) const;

    /*!
     *  Parse the selected expression to decimal.
     *
     *      @param [in] expression  The selected expression.
     *
     *      @return The value of the expression in decimal form.
     */
    QString parseExpressionToDecimal(QString const& expression) const;

    /*!
     *  Parse the selected expression to the selected base number.
     *
     *      @param [in] expression  The selected expression.
     *      @param [in] baseNumber  The selected base number.
     *
     *      @return The value of the expression in the base number form.
     */
    QString parseExpressionToBaseNumber(QString const& expression, int const& baseNumber) const;

    /*!
     *  Transform the selected name to a unique item name.
     *
     *      @param [in] newName     The selected name.
     *
     *      @return A unique name derived from the selected name.
     */
    QString getUniqueName(string const& newName, string const& itemTypeName) const;

private:

    /*!
     *  Check if the selected name is unique.
     *
     *      @param [in] portName    The selected name.
     *
     *      @return True, if the selected name is unique, false otherwise.
     */
    bool nameIsUnique(QString const& portName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Expression parser for solving expressions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Expression formatter, formats the referencing expressions to show parameter names.
    QSharedPointer<ExpressionFormatter> formatter_;

    //! Formatter for values.
    QSharedPointer<ValueFormatter> valueFormatter_;
};

#endif // PARAMETERIZABLEINTERFACE_H
