//-----------------------------------------------------------------------------
// File: ExpressionProxyModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.06.2015
//
// Description:
// Filter model for sorting items by expression value.
//-----------------------------------------------------------------------------

#ifndef EXPRESSIONPROXYMODEL_H
#define EXPRESSIONPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! Filter model for sorting items by expression value.
//-----------------------------------------------------------------------------
class ExpressionProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent  The parent object.
	*/
	ExpressionProxyModel(QSharedPointer<ExpressionParser> expressionParser, QObject *parent);
	
	//! The destructor
	virtual ~ExpressionProxyModel();

    /*!
     *  Sets a column to accept expressions and to be compared by the expression value.
     *
     *      @param [in] column   The column to set as expression column.
     */
    void setColumnToAcceptExpressions(int column);

protected:

	/*! Compares two items by value.
	 *
	 *      @param left [in]    Index identifying the left item.
	 *      @param right [in]   Index identifying the right item.
	 *
	 *      @return True, if left < right, otherwise false.
	*/
	virtual bool lessThan(QModelIndex const& left, QModelIndex const& right) const;

private:
	
	//! No copying
	ExpressionProxyModel(const ExpressionProxyModel& other);
	ExpressionProxyModel& operator=(const ExpressionProxyModel& other);

    //! Parser for solving expressions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The columns accepting expressions.
    QList<int> expressionColumns_;
};

#endif // EXPRESSIONPROXYMODEL_H
