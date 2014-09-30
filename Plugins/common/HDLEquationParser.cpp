//-----------------------------------------------------------------------------
// File: EquationParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.09.2014
//
// Description:
// Parser for simple math equations in HDL e.g. VHDL and Verilog.
//-----------------------------------------------------------------------------

#include "HDLEquationParser.h"
#include "HDLmath.h"

#include <IPXACTmodels/modelparameter.h>

#include <qmath.h>

//-----------------------------------------------------------------------------
// Function: EquationParser::EquationParser()
//-----------------------------------------------------------------------------
HDLEquationParser::HDLEquationParser(QList<QSharedPointer<ModelParameter> > modelParameters) :
    knownValues_(modelParameters)
{

}

//-----------------------------------------------------------------------------
// Function: EquationParser::()
//-----------------------------------------------------------------------------
HDLEquationParser::~HDLEquationParser()
{

}

//-----------------------------------------------------------------------------
// Function: EquationParser::parse()
//-----------------------------------------------------------------------------
int HDLEquationParser::parse(QString const& equation) const
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
QStringList HDLEquationParser::toStringList(QString const& equation) const
{    
    HDLmath::OPERATION_EXP.indexIn(equation);  

    QString firstTerm = HDLmath::OPERATION_EXP.cap(1);
    QString otherTerms = HDLmath::OPERATION_EXP.cap(0).mid(firstTerm.length());

    QStringList listedEquation = QStringList(firstTerm);

    static QRegExp nextTerm = QRegExp("(" + HDLmath::OPERATOR + ")\\s*(\\w+)");
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
bool HDLEquationParser::hasPower(QStringList& listedEquation) const
{
    return listedEquation.indexOf(HDLmath::POWER) != -1;
}

//-----------------------------------------------------------------------------
// Function: EquationParser::solvePower()
//-----------------------------------------------------------------------------
QStringList HDLEquationParser::solvePower(QStringList& listedEquation) const
{
    int powerOperator = listedEquation.indexOf(HDLmath::POWER);        

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
int HDLEquationParser::valueOf(QString const& term) const
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
int HDLEquationParser::valueFromModelParameter(QString const& term) const
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
bool HDLEquationParser::hasMultiplyOrDivide(QStringList& listedEquation) const
{
    return listedEquation.indexOf(HDLmath::MULTIPLYDIVIDE) != -1;
}

//-----------------------------------------------------------------------------
// Function: EquationParser::solveMultiplyOrDivide()
//-----------------------------------------------------------------------------
QStringList HDLEquationParser::solveMultiplyAndDivide(QStringList& listedEquation) const
{
    int multiplyOperator = listedEquation.indexOf(HDLmath::MULTIPLYDIVIDE);

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
QStringList HDLEquationParser::solveAddAndSubtract(QStringList &listedEquation) const
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
