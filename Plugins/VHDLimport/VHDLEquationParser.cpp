//-----------------------------------------------------------------------------
// File: EquationParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.09.2014
//
// Description:
// Parser for simple math equations in VHDL.
//-----------------------------------------------------------------------------

#include "VHDLEquationParser.h"

#include <IPXACTmodels/modelparameter.h>

#include <qmath.h>

namespace
{
    const QRegExp POWER = QRegExp("[/*][/*]");          //! Power operator **.
    const QRegExp MULTIPLYDIVIDE = QRegExp("[/*/]");    //! Multiply * and divide /.

    //! Basic arithmetic operators.
    const QString OPERATOR = "[+-]|" + POWER.pattern() + "|" + MULTIPLYDIVIDE.pattern(); 

    //! Basic arithmetic operations e.g. x + y or x*y.
    const QRegExp OPERATION = QRegExp("(\\w+)(?:\\s*(?:" + OPERATOR + ")\\s*(\\w+))*");
}

//-----------------------------------------------------------------------------
// Function: EquationParser::EquationParser()
//-----------------------------------------------------------------------------
VHDLEquationParser::VHDLEquationParser(QList<QSharedPointer<ModelParameter> > modelParameters) :
    knownValues_(modelParameters)
{

}

//-----------------------------------------------------------------------------
// Function: EquationParser::()
//-----------------------------------------------------------------------------
VHDLEquationParser::~VHDLEquationParser()
{

}

//-----------------------------------------------------------------------------
// Function: EquationParser::parse()
//-----------------------------------------------------------------------------
int VHDLEquationParser::parse(QString const& equation) const
{
    if (equation.isEmpty())
    {
        return -1;
    } 

    // Do not calculate hex (or other base) values.
    if (equation.indexOf(QRegExp("\\d+[#]\\d+[#]")) != -1)
    {
        return -1;
    }

    // Do not calculate logic values e.g. '1' or vectors e.g. "1001".
    if (equation.contains(QRegExp("['\"]")))
    {
        return -1;
    }
    
    QStringList listedEquation = toStringList(equation);

    while (hasPower(listedEquation))
    {
        listedEquation = solvePower(listedEquation);
    }

    while (hasMultiplyOrDivide(listedEquation))
    {
        listedEquation = solveMultiplyAndDivide(listedEquation);
    }

    while (listedEquation.size() > 1)
    {        
        listedEquation = solveAddAndSubtract(listedEquation);
    }

    return valueOf(listedEquation.first());
}

//-----------------------------------------------------------------------------
// Function: EquationParser::toStringList()
//-----------------------------------------------------------------------------
QStringList VHDLEquationParser::toStringList(QString const& equation) const
{    
    OPERATION.indexIn(equation);  

    QString firstTerm = OPERATION.cap(1);
    QString otherTerms = OPERATION.cap(0).mid(firstTerm.length());

    QStringList listedEquation = QStringList(firstTerm);

    static QRegExp nextTerm = QRegExp("(" + OPERATOR + ")\\s*(\\w+)");
    int index = 0;
    while (nextTerm.indexIn(otherTerms, index) != -1)
    {
        QString operation = nextTerm.cap(1);
        QString term = nextTerm.cap(2);

        listedEquation.append(operation);
        listedEquation.append(term);

        index += nextTerm.matchedLength();
    }

    return listedEquation;
}

//-----------------------------------------------------------------------------
// Function: EquationParser::hasPower()
//-----------------------------------------------------------------------------
bool VHDLEquationParser::hasPower(QStringList& listedEquation) const
{
    return listedEquation.indexOf(POWER) != -1;
}

//-----------------------------------------------------------------------------
// Function: EquationParser::solvePower()
//-----------------------------------------------------------------------------
QStringList VHDLEquationParser::solvePower(QStringList& listedEquation) const
{
    int powerOperator = listedEquation.indexOf(POWER);        

    QString leftOperand = listedEquation.at(powerOperator - 1);
    QString rightOperand = listedEquation.at(powerOperator + 1);

    int left = valueOf(leftOperand);
    int right = valueOf(rightOperand);

    int result = qPow(left, right);

    listedEquation.replace(powerOperator - 1, QString::number(result));
    listedEquation.removeAt(powerOperator + 1);
    listedEquation.removeAt(powerOperator);

    return listedEquation;
}

//-----------------------------------------------------------------------------
// Function: valueOf()
//----------------------------------------------------------------------------- 
int VHDLEquationParser::valueOf(QString const& term) const
{
    bool isNumber = false;
    int value = term.toInt(&isNumber);

    if ( !isNumber )
    {
        value = valueFromModelParameter(term);
    }

    return value;
}

//-----------------------------------------------------------------------------
// Function: EquationParser::valueFromModelParameter()
//-----------------------------------------------------------------------------
int VHDLEquationParser::valueFromModelParameter(QString const& term) const
{
    foreach (QSharedPointer<ModelParameter> parameter, knownValues_) 
    {
        if (parameter->getName() == term)
        {
            return parse(parameter->getValue());
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: EquationParser::hasMultiplyOrDivide()
//-----------------------------------------------------------------------------
bool VHDLEquationParser::hasMultiplyOrDivide(QStringList& listedEquation) const
{
    return listedEquation.indexOf(MULTIPLYDIVIDE) != -1;
}

//-----------------------------------------------------------------------------
// Function: EquationParser::solveMultiplyOrDivide()
//-----------------------------------------------------------------------------
QStringList VHDLEquationParser::solveMultiplyAndDivide(QStringList& listedEquation) const
{
    int multiplyOperator = listedEquation.indexOf(MULTIPLYDIVIDE);

    Q_ASSERT(multiplyOperator != 0 && multiplyOperator != listedEquation.length());

    QString operation = listedEquation.at(multiplyOperator);
    QString leftOperand = listedEquation.at(multiplyOperator-1);
    QString rightOperand = listedEquation.at(multiplyOperator+1);

    int left = valueOf(leftOperand);
    int right = valueOf(rightOperand);

    int result = 0;

    if ( operation == "*" )
    {
        result = left * right;
    }
    else if (right != 0)
    {
        result = left / right;
    }

    listedEquation.replace(multiplyOperator - 1, QString::number(result));
    listedEquation.removeAt(multiplyOperator);
    listedEquation.removeAt(multiplyOperator);

    return listedEquation;
}

//-----------------------------------------------------------------------------
// Function: EquationParser::solveAddAndSubtract()
//-----------------------------------------------------------------------------
QStringList VHDLEquationParser::solveAddAndSubtract(QStringList &listedEquation) const
{
    int left = valueOf(listedEquation.value(0));
    int right = valueOf(listedEquation.value(2));

    int result = 0;

    if ( listedEquation.value(1) == "+" )
    {
        result = left + right;
    }
    else if ( listedEquation.value(1) == "-" )
    {
        result = left - right;
    }

    listedEquation.replace(0, QString::number(result));
    listedEquation.removeAt(1);
    listedEquation.removeAt(1);

    return listedEquation;
}
