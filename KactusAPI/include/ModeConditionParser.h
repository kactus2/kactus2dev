//-----------------------------------------------------------------------------
// File: ModeConditionParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 16.10.2023
//
// Description:
// Parser for Mode condition expressions.
//-----------------------------------------------------------------------------

#ifndef MODECONDITIONPARSER_H
#define MODECONDITIONPARSER_H

#include "IPXactSystemVerilogParser.h"

#include "KactusAPI/KactusAPIGlobal.h"

#include <IPXACTmodels/Component/Mode.h>

#include <IPXACTmodels/utilities/Search.h>

#include <QString>
#include <QMap>

//-----------------------------------------------------------------------------
//! Parser for Mode condition expressions.
//-----------------------------------------------------------------------------
class KACTUS2_API ModeConditionParser : public IPXactSystemVerilogParser
{
public:

	//! The constructor.
    ModeConditionParser(QSharedPointer<ParameterFinder> finder,
        PortSlice::List portSlices,
        FieldSlice::List fieldSlices,
        Mode::List modes);

	//! The destructor.
    ~ModeConditionParser() override = default;

    // Disable copying.
    ModeConditionParser(ModeConditionParser const& rhs) = delete;
    ModeConditionParser& operator=(ModeConditionParser const& rhs) = delete;

protected:

    /*!
     *  Checks if the given expression is a symbol e.g. reference.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is a symbol, otherwise false.
     */
    bool isSymbol(QString const& expression) const final;

    /*!
     *  Finds the value for given symbol.
     *
     *      @param [in] symbol  The symbol whose value to find.
     *
     *      @return The found symbol value.
     */
    QString findSymbolValue(QString const& symbol) const override;

    /*!
     *  Checks if the given token is a unary operator.
     *
     *      @param [in] expression   The token to check.
     *
     *      @return True, if the token is a unary operator, otherwise false.
     */
    bool isUnaryOperator(QString const& token) const final;

    /*!
     *  Solves a binary operation.
     *
     *      @param [in] operation    The operation to solve.
     *      @param [in] term         The term for the operation.
     *
     *      @return The result of the operation.
     */
    QString solveUnary(QString const& operation, QString const& term) const final;

private:

    /*!
     *  Check if the given expression is a valid reference within a mode.
     *
     *      @param [in] expression  The expression to check.
     *
     *      @return True, if the expression is a valid reference, otherwise false.
     */
    bool isModeReference(QString const& expression) const;


    /*!
     *  Solve condition to a value.
     *
     *      @param [in] reference       The reference to solve.
     *      @param [in] availableItems  The available items to reference.
     *
     *      @return 1, if the reference is to a valid item, otherwise x.
     */
    template <typename T> 
    QString solveCondition(QString const& reference, QSharedPointer<QList<QSharedPointer<T> > > availableItems) const
    {
        if (auto item = Search::findByName(reference, *availableItems); item.isNull())
        {
            return QStringLiteral("x");
        }

        return QStringLiteral("1");
    };

    //! The list of available port slices in expressions.
    PortSlice::List portSlices_;

    //! The list of available field slices in expressions.
    FieldSlice::List fieldSlices_;

    //! The list of available modes in expressions.
    Mode::List modes_;
};

#endif // MODECONDITIONPARSER_H
