#ifndef SINGLEREGISTERFILEEDITOR_H
#define SINGLEREGISTERFILEEDITOR_H

#include "registerfileeditor.h"
#include <editors/ComponentEditor/itemeditor.h>
#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/RegisterFile.h>

class LibraryInterface;
class ParameterFinder;
class ExpressionFormatter;
class ExpressionEditor;
class ExpressionParser;
class RegisterFileValidator;
class SingleRegisterFileEditor : public ItemEditor
{
    Q_OBJECT

public:

    SingleRegisterFileEditor(QSharedPointer<RegisterFile> selectedRegisterFile,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<RegisterFileValidator> registerFileValidator,
        QWidget* parent = 0);


    virtual ~SingleRegisterFileEditor();

    virtual void refresh();

protected:

    virtual void showEvent(QShowEvent* event);

private slots:
    void onOffsetEdited();
    void onRangeEdited();
    void onDimensionEdited();
    void onIsPresentEdited();

private:

    SingleRegisterFileEditor(const SingleRegisterFileEditor& other);
    SingleRegisterFileEditor& operator=(const SingleRegisterFileEditor& other);

    void setupLayout();
    void connectSignals();

    QString formattedValueFor(QString const& expression) const;
    void changeExpressionEditorsSignalBlockStatus(bool blockStatus) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The selected register.
    QSharedPointer<RegisterFile> registerFile_;

    //! The name editor.
    NameGroupEditor nameEditor_;

    //! The editor for the registers fields.
    RegisterFileEditor* registerFileEditor_;

    ExpressionEditor* offsetEditor_;
    ExpressionEditor* rangeEditor_;
    ExpressionEditor* dimensionEditor_;
    ExpressionEditor* isPresentEditor_;


    QSharedPointer<ExpressionParser> expressionParser_;
    QSharedPointer<RegisterFileValidator> registerFileValidator_;
};

#endif // SINGLEREGISTERFILEEDITOR_H
