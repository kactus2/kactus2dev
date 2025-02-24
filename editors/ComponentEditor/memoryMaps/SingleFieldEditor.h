//-----------------------------------------------------------------------------
// File: SingleFieldEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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

#include <QSharedPointer>

class ResetsEditor;
class FieldEditor;
class ExpressionEditor;
class BooleanComboBox;
class AccessComboBox;
class ModWriteComboBox;
class ReadActionComboBox;
class TestConstraintComboBox;
class ParameterFinder;
class ExpressionParser;

class Register;
class Field;
class Component;
class FieldValidator;
class FieldInterface;
class FieldAccessPoliciesEditor;

#include <QComboBox>

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
	 *    @param [in] fieldItem           The field being edited.
	 *    @param [in] component           The component being edited.
	 *    @param [in] handler             The instance managing the library.
     *    @param [in] parameterFinder     The instance for finding parameter references.
     *    @param [in] expressionParser    The expression parser to use.
     *    @param [in] fieldValidator      The used field validator.
     *    @param [in] fieldInterface      Interface for fields.
     *    @param [in] containingRegister  Register containing the edited field.
	 *    @param [in] parent              The parent of this editor.
	 */
    SingleFieldEditor(QSharedPointer<Field> fieldItem,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<FieldValidator> fieldValidator,
        FieldInterface* fieldInterface,
        QSharedPointer<Register> containingRegister,
        QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~SingleFieldEditor() = default;

    /*!
	 *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:
    void addressingChanged();

public slots:

    /*
     *  Handles field name change from register editor.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void onFieldNameChanged(QString const& oldName, QString const& newName);

    /*
     *  Handles field name change from name editor.
     */
    void onFieldNameChanged();

protected:

    /*!
	 *  informs of the required help document.
	 *
	 *    @param [in] event   The show event.
	 */
	virtual void showEvent(QShowEvent* event);

private slots:

    /*!
     *  Sets the edited offset value for the field.
     */
    void onOffsetEdited();

    /*!
     *  Sets the edited width value for the field.
     */
    void onWidthEdited();

    /*!
     *  Sets the selected volatile value for the field.
     *
     *    @param [in] newVolatileValue    The selected volatile value.
     */
    void onVolatileSelected(QString const& newVolatileValue);

    /*!
     *  Sets the selected access value for the field.
     *
     *    @param [in] newAccessValue      The selected access value.
     */
    void onAccessSelected(QString const& newAccessValue);

    /*!
     *  Sets the selected modified write value for the field.
     *
     *    @param [in] newModWriteValue    The selected modified write value.
     */
    void onModifiedWriteSelected(QString const& newModWriteValue);

    /*!
     *  Sets the selected read action value for the field.
     *
     *    @param [in] newReadActionValue  The selected read action value.
     */
    void onReadActionSelected(QString const& newReadActionValue);

    /*!
     *  Sets the selected testable value for the field.
     *
     *    @param [in] newTestableValue    The selected testable value.
     */
    void onTestableSelected(QString const& newTestableValue);

    /*!
     *  Sets the selected test constrained value for the field.
     *
     *    @param [in] newTestConstrainedValue     The selected test constrained value.
     */
    void onTestConstrainedSelected(QString const& newTestConstrainedValue);

    /*!
     *  Sets the edited is present value for the field.
     */
    void onIsPresentEdited();

    /*!
     *  Sets the edited reserved value for the field.
     */
    void onReservedEdited();

    /*!
     *  Sets the edited field ID value for the field.
     */
    void onFieldIdChanged();

    /*!
     *  Sets the selected write constraint value for the field.
     *
     *    @param [in] newIndex    The index of the selected write constraint value.
     */
    void onWriteConstraintSelected(int newIndex);

    /*!
     *  Sets the edited write constraint minimum value for the field.
     *
     *    @param [in] newWriteConstraintMin   The edited write constraint minimum value.
     */
    void onWriteConstraintMinimumEdited();

    /*!
     *  Sets the edited write constraint maximum value for the field.
     *
     *    @param [in] newWriteConstraintMax   The edited write constraint maximum value.
     */
    void onWriteConstraintMaximumEdited();

private:
	
	//! No copying. No assignment.
    SingleFieldEditor(const SingleFieldEditor& other);
    SingleFieldEditor& operator=(const SingleFieldEditor& other);

    /*!
     *  Change the signal blocking status of the expression editors in this editor.
     *
     *    @param [in] blockStatus     The new signal blocking status for the expression editors.
     */
    void changeExpressionEditorSignalBlockStatus(bool blockStatus) const;

    /*!
     *  Calculates the value of an expression.
     *
     *    @param [in] expression  The expression to be calculated.
     *
     *    @return The calculated value of an expression.
     */
    QString formattedValueFor(QString const& expression) const;

    /*!
     *  Set the status of  write constraint minimum and maximum editors.
     *
     *    @param [in] writeConstraintIndex    The index of the write constraint value.
     */
    void setWriteMinMaxConstraintEnableStatus(int writeConstraintIndex) const;
    
    /*!
     *  Connect the signals of the different editors in this editor.
     */
    void connectSignals();

    /*!
     *  Sets the layout for this editor.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name group editor.
    NameGroupEditor nameEditor_;

    //! The resets editor.
    ResetsEditor* resetsEditor_;

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

    //! The presence editor.
    ExpressionEditor* isPresentEditor_;

    //! The reserved editor.
    ExpressionEditor* reservedEditor_;

    //! The editor for fieldID.
    QLineEdit* fieldIdEditor_;

    //! The expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The write constraint selector.
    QComboBox* writeConstraintEditor_;

    //! The write constraint minimum value editor.
    ExpressionEditor* writeConstraintMinLimit_;

    //! The write constraint maximum value editor.
    ExpressionEditor* writeConstraintMaxLimit_;

    //! Name of the edited field.
    std::string fieldName_;

    //! The used field validator.
    QSharedPointer<FieldValidator> fieldValidator_;

    //! Interface for fields.
    FieldInterface* fieldInterface_;

    //! Register containing the edited field.
    QSharedPointer<Register> containingRegister_;

    //! The editor for field access policies.
    FieldAccessPoliciesEditor* accessPoliciesEditor_;
};

#endif // SINGLEFIELDEDITOR_H
