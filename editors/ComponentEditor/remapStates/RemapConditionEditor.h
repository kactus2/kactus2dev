//-----------------------------------------------------------------------------
// File: RemapConditionEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.04.2015
//
// Description:
// Editor used to edit the details of remap conditions in a remap state.
//-----------------------------------------------------------------------------

#ifndef REMAPCONDITIONEDITOR_H
#define REMAPCONDITIONEDITOR_H

#include "RemapConditionModel.h"
#include "RemapConditionDelegate.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <IPXACTmodels/remapport.h>
#include <IPXACTmodels/component.h>

#include <QGroupBox>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
//! Editor used to edit the details of remap conditions in a remap state.
//-----------------------------------------------------------------------------
class RemapConditionEditor : public QGroupBox 
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] remapPorts              The list containing the remap ports of this remap state.
     *      @param [in] component               The component whose remap state is being edited.
     *      @param [in] parameterFinder         The parmaeter finder used to search the components parameters.
     *      @param [in] expressionFormatter     Changes the parameter ids to names in references.
     *      @param [in] parent                  Pointer to the owner of this editor.
     */
    RemapConditionEditor(QSharedPointer<QList<QSharedPointer<RemapPort> > > remapPorts,
        QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QWidget* parent);

    /*!
     *  The destructor.
     */
    virtual ~RemapConditionEditor();

	/*!
	 *  Check for the validity of the edited remap state.
	 *
     *      @return True, if the remap state is valid, false otherwise.
	 */
	virtual bool isValid() const;

	/*!
	 *  Restore the changes made in the editor.
	 */
	virtual void refresh();

signals:

	/*!
	 *  Emitted when contents of the editor change.
	 */
	void contentChanged();

	/*!
	 *  Prints an error message to the user.
	 *
	 *      @param [in] msg     The error message.
	 */
	void errorMessage(const QString& msg) const;

	/*!
	 *  Prints a notification to the user.
	 *
	 *      @param [in] msg     The notification message.
	 */
	void noticeMessage(const QString& msg) const;

    /*!
     *  Increase the amount of references to the parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to the parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void decreaseReferences(QString id);

private:
	//! No copying
    RemapConditionEditor(const RemapConditionEditor& other);

	//! No assignment
    RemapConditionEditor& operator=(const RemapConditionEditor& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The view that displays the conditions.
    EditableTableView* view_;

    //! The model that holds the data to be displayed to the user.
    RemapConditionModel* model_;

    //! The delegate for the view.
    RemapConditionDelegate* delegate_;

    //! The sort filter for the model.
    QSortFilterProxyModel* proxy_;

    //! Pointer to the component.
    QSharedPointer<Component> component_;
};

#endif // REMAPCONDITIONEDITOR_H
