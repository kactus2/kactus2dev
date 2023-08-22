//-----------------------------------------------------------------------------
// File: RegionsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.6.2023
//
// Description:
// Delegate class for CPU regions.
//-----------------------------------------------------------------------------

#ifndef REGIONSDELEGATE_H
#define REGIONSDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <KactusAPI/include/ParameterFinder.h>

#include <QCompleter>

//-----------------------------------------------------------------------------
// Delegate class for CPU regions.
//-----------------------------------------------------------------------------
class RegionsDelegate : public ExpressionDelegate
{
    Q_OBJECT

public:
    /*!
     *  The constructor.
     *
     *      @param [in] parameterNameCompleter      The completer to use for parameter names in expression editor.
     *      @param [in] parameterFinder             The parameter finder to use for for expression editor.
     *      @param [in] parent                      The parent object.
     */
    RegionsDelegate(QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> parameterFinder,
        QObject* parent);

    //! The destructor.
    virtual ~RegionsDelegate() = default;

    // Disable copying.
    RegionsDelegate(RegionsDelegate const& rhs) = delete;
    RegionsDelegate& operator=(RegionsDelegate const& rhs) = delete;

protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column   The column to check.
     *
     *      @return True, if the cells in the column allow expressions, otherwise false.
     */
    bool columnAcceptsExpression(int column) const final;

    //! Gets the description column.
    int descriptionColumn() const final;

};

#endif // REGIONSDELEGATE_H
