//-----------------------------------------------------------------------------
// File: SingleRegisterFileEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// Editor for a single register file item in component editor tree.
//-----------------------------------------------------------------------------
#ifndef SINGLEREGISTERFILEEDITOR_H
#define SINGLEREGISTERFILEEDITOR_H

#include "registerfileeditor.h"

#include <editors/ComponentEditor/itemeditor.h>
#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/RegisterFile.h>

class AddressBlockEditor; 
class LibraryInterface;
class ParameterFinder;
class ExpressionFormatter;
class ExpressionEditor;
class ExpressionParser;
class RegisterFileValidator;
class RegisterInterface;
class AccessPoliciesEditor;

//-----------------------------------------------------------------------------
//! Editor for a single register file item in component editor tree.
//-----------------------------------------------------------------------------
class SingleRegisterFileEditor : public ItemEditor
{
    Q_OBJECT

public:

    /*!
     * The constructor.
     *
     *    @param [in] registerInterface       Interface for registers.
     *    @param [in] registerFile            The register file to edit.
     *    @param [in] component               The component containing the register file.
     *    @param [in] handler                 The available IP-XACT library.
     *    @param [in] parameterFinder         Finder for parameters.
     *    @param [in] expressionFormatter     Formatter for expressions.
     *    @param [in] registerFileValidator   Validator for register files.
     *    @param [in] parent                  The parent widget.
    */
    SingleRegisterFileEditor(RegisterInterface* registerInterface,
        QSharedPointer<RegisterFile> registerFile,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<RegisterFileValidator> registerFileValidator,
        QWidget* parent = 0);

    //! The destructor.
    virtual ~SingleRegisterFileEditor() = default;

    //! No copying.
    SingleRegisterFileEditor(const SingleRegisterFileEditor& other) = delete;
    SingleRegisterFileEditor& operator=(const SingleRegisterFileEditor& other) = delete;

    //! Refresh the editor.
    virtual void refresh() override final;

signals:
    
    void addressingChanged();

    void childAddressingChanged(int index);

    void childGraphicsChanged(int index);

signals:

    /*
     *  Informs of register name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void registerNameChanged(QString const& oldName, QString const& newName);

protected:

    virtual void showEvent(QShowEvent* event);

private slots:

    //! Called when offset is changed.
    void onOffsetEdited();

    //! Called when range is changed.
    void onRangeEdited();

    //! Called when dimension is changed.
    void onDimensionEdited();

    //! Called when stride is changed.
    void onStrideEdited();

    //! Called when isPresent is changed.
    void onIsPresentEdited();

private:

    //! Set the editor layout.
    void setupLayout();

    //! Connect the editor signals.
    void connectSignals();

    
    /*!
     * Get the formatted value of a given expression.
     *
     *    @param [in] expression  The expression to format.
     *
     *    @return The formatted value.
     */
    QString formattedValueFor(QString const& expression) const;

    /*!
     * Block/unblock signals from expression editor.
     *
     *    @param [in] blockStatus  If true, signals are blocked, otherwise signals are enabled.
     */
     void changeExpressionEditorsSignalBlockStatus(bool blockStatus) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The register file edited in the editor.
    QSharedPointer<RegisterFile> registerFile_;

    //! The name editor.
    NameGroupEditor nameEditor_;

    //! The editor for registers.
    AddressBlockEditor* registersEditor_;

    //! The editor for the register files.
    RegisterFileEditor* registerFileEditor_;

    //! The editor for access policies.
    AccessPoliciesEditor* accessPoliciesEditor_;

    //! The editor for register file offset.
    ExpressionEditor* offsetEditor_;
    
    //! The editor for register file range.
    ExpressionEditor* rangeEditor_;

    //! The editor for register file dimension.
    ExpressionEditor* dimensionEditor_;

    //! The register file stride editor.
    ExpressionEditor* strideEditor_;

    //! The editor for register file isPresent property.
    ExpressionEditor* isPresentEditor_;

    //! Parser for expression.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Validator for register files.
    QSharedPointer<RegisterFileValidator> registerFileValidator_;
};

#endif // SINGLEREGISTERFILEEDITOR_H
