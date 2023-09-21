//-----------------------------------------------------------------------------
// File: SingleRegisterEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
class AccessComboBox;
class BooleanComboBox;
class ExpressionEditor;
class ExpressionParser;
class Register;
class RegisterValidator;
class RegisterInterface;
class AddressBlock;
class RegisterBase;
class AccessPoliciesEditor;

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
	 *      @param [in] selectedRegister        The register being edited.
	 *      @param [in] component               The component being edited.
	 *      @param [in] handler                 The instance that manages the library.
	 *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionParser        The expression parser to use.
     *      @param [in] registerInterface       Interface for registers.
     *      @param [in] containingRegisterData  Register data containing the edited register.
	 *      @param [in] parent                  The parent of this editor.
	 */
    SingleRegisterEditor(QSharedPointer<Register> selectedRegister,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        RegisterInterface* registerInterface,
        QSharedPointer<QList<QSharedPointer<RegisterBase> > > containingRegisterData,
        QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~SingleRegisterEditor() = default;

    //! No copying
    SingleRegisterEditor(const SingleRegisterEditor& other) = delete;

    //! No assignment
    SingleRegisterEditor& operator=(const SingleRegisterEditor& other) = delete;

    /*!
	 *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:
    void childGraphicsChanged(int);

    void addressingChanged();

    void childAddressingChanged(int);

    /*
     *  Informs of field name change.
     *
     *      @param [in] oldName     The old name.
     *      @param [in] newName     The new name.
     */
    void fieldNameChanged(QString const& oldName, QString const& newName);


public slots:

    /*
     *  Handles register name change from address block editor.
     *
     *      @param [in] oldName     The old name.
     *      @param [in] newName     The new name.
     */
    void onRegisterNameChanged(QString const& oldName, QString const& newName);

    /*
     *  Handles register name change from name editor.
     */
    void onRegisterNameChanged();

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

    //! Name of the selected register.
    std::string registerName_;

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

    //! Register data containing the edited register.
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > containingRegisterData_;

    //! Interface for registers.
    RegisterInterface* registerInterface_;

    //! The editor for access policies.
    AccessPoliciesEditor* accessPoliciesEditor_;
};

#endif // SINGLEREGISTEREDITOR_H
