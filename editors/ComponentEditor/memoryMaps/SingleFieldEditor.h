//-----------------------------------------------------------------------------
// File: SingleFieldEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.04.2015
//
// Description:
// Editor for editing the details of a single field.
//-----------------------------------------------------------------------------

#ifndef SINGLEFIELDEDITOR_H
#define SINGLEFIELDEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <IPXACTmodels/field.h>
#include <IPXACTmodels/component.h>

#include <QSharedPointer>

class FieldEditor;
class ExpressionEditor;
class BooleanComboBox;
class AccessComboBox;
class ModWriteComboBox;
class ReadActionComboBox;
class TestConstraintComboBox;
class ParameterFinder;
class WriteValueConstraintComboBox;
class ExpressionParser;
//-----------------------------------------------------------------------------
//! Editor for editing the details of a single register.
//-----------------------------------------------------------------------------
class SingleFieldEditor : public ItemEditor
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] field               The field being edited.
	 *      @param [in] component           The component being edited.
	 *      @param [in] handler             The instance managing the library.
     *      @param [in] parameterFinder     The instance for finding parameter references.
     *      @param [in] expressionParser    The expression parser to use.
	 *      @param [in] parent              The parent of this editor.
	 */
	SingleFieldEditor(QSharedPointer<Field> field,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~SingleFieldEditor();

	/*!
	 *  Check for the validity of the editor.
	 *
     *      @return True, if the editor is in a valid state, false otherwise.
	 */
	virtual bool isValid() const;

    /*!
	 *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

protected:

    /*!
	 *  informs of the required help document.
	 *
	 *      @param [in] event   The show event.
	 */
	virtual void showEvent(QShowEvent* event);

private slots:

    /*!
     *  Sets the edited offset value for the field.
     */
    void onOffsetEdited();

    /*!
     *  Sets the edited width value for the field].
     */
    void onWdithEdited();

    /*!
     *  Sets the selected volatile value for the field.
     *
     *      @param [in] newVolatileValue    The selected volatile value.
     */
    void onVolatileSelected(QString const& newVolatileValue);

    /*!
     *  Sets the selected access value for the field.
     *
     *      @param [in] newAccessValue      The selected access value.
     */
    void onAccessSelected(QString const& newAccessValue);

    /*!
     *  Sets the selected modified write value for the field.
     *
     *      @param [in] newModWriteValue    The selected modified write value.
     */
    void onModifiedWriteSelected(QString const& newModWriteValue);

    /*!
     *  Sets the selected read action value for the field.
     *
     *      @param [in] newReadActionValue  The selected read action value.
     */
    void onReadActionSelected(QString const& newReadActionValue);

    /*!
     *  Sets the selected testable value for the field.
     *
     *      @param [in] newTestableValue    The selected testable value.
     */
    void onTestableSelected(QString const& newTestableValue);

    /*!
     *  Sets the selected test constrained value for the field.
     *
     *      @param [in] newTestConstrainedValue     The selected test constrained value.
     */
    void onTestConstrainedSelected(QString const& newTestConstrainedValue);

    /*!
     *  Sets the edited is present value for the field.
     */
    void onIsPresentEdited();

    /*!
     *  Sets the selected write constraint value for the field.
     *
     *      @param [in] newIndex    The index of the selected write constraint value.
     */
    void onWriteConstraintSelected(int newIndex);

    /*!
     *  Sets the edited write constraint minimum value for the field.
     *
     *      @param [in] newWriteConstraintMin   The edited write constraint minimum value.
     */
    void onWriteConstraintMinimumEdited(QString const& newWriteConstraintMin);

    /*!
     *  Sets the edited write constraint maximum value for the field.
     *
     *      @param [in] newWriteConstraintMax   The edited write constraint maximum value.
     */
    void onWriteConstraintMaximumEdited(QString const& newWriteConstraintMax);

    /*!
     *  Sets the edited reset value for the field.
     *
     *      @param [in] newResetValue   The new reset value.
     */
    void onResetValueEdited();

    /*!
     *  Sets the edited reset mask for the field.
     *
     *      @param [in] newResetMask    The new reset mask.
     */
    void onResetMaskEdited();

private:
	
	//! No copying
    SingleFieldEditor(const SingleFieldEditor& other);

	//! No assignment
    SingleFieldEditor& operator=(const SingleFieldEditor& other);

    /*!
     *  Sets the layout for this editor.
     */
    void setupLayout();

    /*!
     *  Connect the signals of the different editors in this editor.
     */
    void connectSignals();

    /*!
     *  Change the signal blocking status of the expression editors in this editor.
     *
     *      @param [in] blockStatus     The new signal blocking status for the expression editors.
     */
    void changeExpressionEditorSignalBlockStatus(bool blockStatus) const;

    /*!
     *  Calculates the value of an expression.
     *
     *      @param [in] expression  The expression to be calculated.
     *
     *      @return The calculated value of an expression.
     */
    QString formattedValueFor(QString const& expression) const;

    /*!
     *  Set the status of  write constraint minimum and maximum editors.
     *
     *      @param [in] writeConstraintIndex    The index of the write constraint value.
     */
    void setWriteMinMaxConstraintEnableStatus(int writeConstraintIndex) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name group editor.
    NameGroupEditor nameEditor_;

    //! The enumerations editor.
    FieldEditor* enumerationsEditor_;

    //! The offset editor, accepts expressions.
    ExpressionEditor* offsetEditor_;

    //! The width editor, accepts expressions..
    ExpressionEditor* widthEditor_;

    //! The volatility selector.
    BooleanComboBox* volatileEditor_;

    //! The access selector.
    AccessComboBox* accessEditor_;

    //! The modified write value selector.
    ModWriteComboBox* modifiedWriteValueEditor_;

    //! The read action selector.
    ReadActionComboBox* readActionEditor_;

    //! The testable value selector.
    BooleanComboBox* testableEditor_;

    //! The test constrained value selector.
    TestConstraintComboBox* testConstrainedEditor_;

    //! The is present editor, accepts expressions..
    ExpressionEditor* isPresentEditor_;

    //! The expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The write constraint selector.
    WriteValueConstraintComboBox* writeConstraintEditor_;

    //! The write constraint minimum value editor.
    QLineEdit* writeConstraintMinLimit_;

    //! The write constraint maximum value editor.
    QLineEdit* writeConstraintMaxLimit_;

    //! Editor for the reset value of a field.
    QLineEdit* resetValueEditor_;

    //! Editor for the reset mask of a field.
    QLineEdit* resetMaskEditor_;

    //! The field being edited.
    QSharedPointer<Field> field_;
};

#endif // SINGLEFIELDEDITOR_H
