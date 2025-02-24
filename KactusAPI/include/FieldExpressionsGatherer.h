//-----------------------------------------------------------------------------
// File: FieldExpressionsGatherer.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from a field.
//-----------------------------------------------------------------------------

#ifndef FIELDEXPRESSIONSGATHERER_H
#define FIELDEXPRESSIONSGATHERER_H

class Field;

#include <QSharedPointer>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Gathers expressions from a field.
//-----------------------------------------------------------------------------
class FieldExpressionsGatherer
{

public:

    /*!
     *  The constructor.
     */
    FieldExpressionsGatherer();

    /*!
     *  The destructor.
     */
    virtual ~FieldExpressionsGatherer();

    /*!
     *  Get the expressions from a given field.
     *
     *    @param [in] field   The given field.
     *
     *    @return A list containing all the expressions used in a field.
     */
    QStringList getExpressions(QSharedPointer<Field> field) const;

private:
	
	//! No copying
    FieldExpressionsGatherer(const FieldExpressionsGatherer& other);

	//! No assignment
    FieldExpressionsGatherer& operator=(const FieldExpressionsGatherer& other);

};

#endif // FIELDEXPRESSIONSGATHERER_H
