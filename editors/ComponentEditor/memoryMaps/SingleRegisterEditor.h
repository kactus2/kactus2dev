//-----------------------------------------------------------------------------
// File: SingleRegisterEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.04.2015
//
// Description:
// Editor for editing the details of a single register.
//-----------------------------------------------------------------------------

#ifndef SINGLEREGISTEREDITOR_H
#define SINGLEREGISTEREDITOR_H

#include "registereditor.h"

#include <editors/ComponentEditor/itemeditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QLineEdit>

class LibraryInterface;
class ParameterFinder;
class ExpressionFormatter;
class AccessComboBox;
class BooleanComboBox;
class ExpressionEditor;
class ExpressionParser;
class Register;
class RegisterValidator;
//-----------------------------------------------------------------------------
//! Editor for editing the details of a single register.
//-----------------------------------------------------------------------------
class SingleRegisterEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] singleRegister          The register being edited.
	 *      @param [in] component               The component being edited.
	 *      @param [in] handler                 The instance that manages the library.
	 *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] expressionParser        The expression parser to use.
     *      @param [in] registerValidator       Validator used for registers.
	 *      @param [in] parent                  The parent of this editor.
	 */
    SingleRegisterEditor(QSharedPointer<Register> selectedRegister,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<RegisterValidator> registerValidator,
        QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~SingleRegisterEditor();

    /*!
	 *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

protected:

	/*!
	 *  After a show event, fetches the help file for the editor.
	 *
	 *      @param [in] event   The show event.
	 */
	virtual void showEvent(QShowEvent* event);

private slots:
    
    /*!
     *  Save the edited offset to the register.
     */
    void onOffsetEdited();

    /*!
     *  Save the edited size to the register.
     */
    void onSizeEdited();

    /*!
     *  Save the edited dimension to the register.
     */
    void onDimensionEdited();
    
    /*!
     *  Save the edited is present to the register.
     */
    void onIsPresentEdited();

    /*!
     *  Save the selected volatile value to the register.
     *
     *      @param [in] newVolatileValue    The new volatile value.
     */
    void onVolatileSelected(QString const& newVolatileValue);

    /*!
     *  Save the selected access value to the register.
     *
     *      @param [in] newAccessValue  The new access value.
     */
    void onAccessSelected(QString const& newAccessValue);

private:

	//! No copying
    SingleRegisterEditor(const SingleRegisterEditor& other);

	//! No assignment
    SingleRegisterEditor& operator=(const SingleRegisterEditor& other);

    /*!
     *  Setup the layout for this editor.
     */
    void setupLayout();

    /*!
     *  Connect the signals for this editor.
     */
    void connectSignals();

    /*!
     *  Calculate the value for the expression.
     *
     *      @param [in] expression  The expression to be calculated.
     *
     *      @return The calculated value of the expression.
     */
    QString formattedValueFor(QString const& expression) const;

    /*!
     *  Change the signal blocking status of the expression editors.
     *
     *      @param [in] blockStatus     The new status of signal blocking.
     */
    void changeExpressionEditorsSignalBlockStatus(bool blockStatus) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The selected register.
    QSharedPointer<Register> selectedRegister_;

    //! The name editor.
    NameGroupEditor nameEditor_;

    //! The editor for the registers fields.
    RegisterEditor* fieldsEditor_;

    //! The offset editor.
    ExpressionEditor* offsetEditor_;

    //! The size editor.
    ExpressionEditor* sizeEditor_;

    //! The dimension editor.
    ExpressionEditor* dimensionEditor_;

    //! The is present editor, accepts expressions.
    ExpressionEditor* isPresentEditor_;

    //! The volatility selector.
    BooleanComboBox* volatileEditor_;

    //! The access selector.
    AccessComboBox* accessEditor_;

    //! The expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The used register validator.
    QSharedPointer<RegisterValidator> registerValidator_;
};

#endif // SINGLEREGISTEREDITOR_H
