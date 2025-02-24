//-----------------------------------------------------------------------------
// File: FieldAccessPoliciesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Editor for field access policies.
//-----------------------------------------------------------------------------

#ifndef FIELDACCESSPOLICIESEDITOR_H
#define FIELDACCESSPOLICIESEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>

class FieldInterface;
class ParameterFinder;
class ExpressionParser;
class QSortFilterProxyModel;

class FieldAccessPoliciesEditor : public QGroupBox
{
    Q_OBJECT

public:

    FieldAccessPoliciesEditor(QString const& fieldName, FieldInterface* fieldInterface, 
        QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
        QWidget* parent);

    virtual ~FieldAccessPoliciesEditor() = default;

    FieldAccessPoliciesEditor(FieldAccessPoliciesEditor& other) = delete;
    FieldAccessPoliciesEditor& operator=(FieldAccessPoliciesEditor& other) = delete;

    void refresh();

signals:

    /*!
     *	Emitted whenever a field access policy has been edited.
     */
    void contentChanged();


    /*!
     *  Increase the amount of references to a parameter with a matching id.
     *
     *    @param [in] id      Id of the parameter, whose references are being increased.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the amount of references to a parameter with a matching id.
     *
     *    @param [in] id      Id of the parameter, whose references are being increased.
     */
    void decreaseReferences(QString const& id);


private:

    //! The table view to display the field access policies.
    EditableTableView* view_;

    //! Proxy to sort field access policies.
    QSortFilterProxyModel* proxy_;
};

#endif // FIELDACCESSPOLICIESEDITOR_H