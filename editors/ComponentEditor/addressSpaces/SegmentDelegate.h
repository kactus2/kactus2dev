//-----------------------------------------------------------------------------
// File: SegmentDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 11.05.2015
//
// Description:
// Delegate class for segments within an address space.
//-----------------------------------------------------------------------------

#ifndef SEGMENTDELEGATE_H
#define SEGMENTDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <KactusAPI/include/ParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>

//-----------------------------------------------------------------------------
// Function: SegmentDelegate::SegmentDelegate()
//-----------------------------------------------------------------------------
class SegmentDelegate : public ExpressionDelegate
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
    SegmentDelegate(QCompleter* parameterNameCompleter, QSharedPointer<ParameterFinder> parameterFinder, 
        QObject* parent);

    //! The destructor.
    virtual ~SegmentDelegate() = default;

    // Disable copying.
    SegmentDelegate(SegmentDelegate const& rhs) = delete;
    SegmentDelegate& operator=(SegmentDelegate const& rhs) = delete;

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

#endif // SEGMENTDELEGATE_H
