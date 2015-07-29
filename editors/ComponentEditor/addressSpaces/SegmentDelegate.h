//-----------------------------------------------------------------------------
// File: SegmentDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.05.2015
//
// Description:
// Delegate class for segments within an address space.
//-----------------------------------------------------------------------------

#ifndef SEGMENTDELEGATE_H
#define SEGMENTDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>
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
    ~SegmentDelegate();

protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column   The column to check.
     *
     *      @return True, if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

    //! Gets the description column.
    virtual int descriptionColumn() const;

private:

    // Disable copying.
    SegmentDelegate(SegmentDelegate const& rhs);
    SegmentDelegate& operator=(SegmentDelegate const& rhs);  


};

#endif // SEGMENTDELEGATE_H
