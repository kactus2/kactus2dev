//-----------------------------------------------------------------------------
// File: ApiFunctionEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.4.2012
//
// Description:
// Editor for API functions.
//-----------------------------------------------------------------------------

#ifndef APIFUNCTIONEDITOR_H
#define APIFUNCTIONEDITOR_H

#include "ApiFunctionParameterModel.h"
#include "ApiFunctionModel.h"
#include "ApiFunctionParameterDelegate.h"

#include <common/views/EditableTableView/editabletableview.h>
#include <common/widgets/listManager/listmanager.h>

#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QSortFilterProxyModel>

class ApiFunction;
class ApiDefinition;

//-----------------------------------------------------------------------------
//! Property value editor.
//-----------------------------------------------------------------------------
class ApiFunctionEditor : public QGroupBox
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    ApiFunctionEditor(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ApiFunctionEditor();

    /*!
     *  Sets the linked COM definition.
     *
     *      @param [in] comDefinition The COM definition to set.
     */
    void setComDefinition(QSharedPointer<ComDefinition const> comDefinition);

    /*!
     *  Updates the data types suggestions list.
     *
     *      @param [in] dataTypes The list of custom data types.
     */
    void updateDataTypes(QStringList const& dataTypes);

    /*!
     *  Restores the API functions from the given API definition.
     */
    void restore(ApiDefinition const& apiDef);

    /*!
     *  Applies the API function changes to the given API definition.
     */
    void save(ApiDefinition& apiDef);

public slots:
    //! Called when the selection function has changed.
    void selectionFunctionChanged(QModelIndex const& cur);

    //! Called when the function description has been edited.
    void descriptionEdited(QString const& text);

    //! Called when the return type has been edited.
    void returnTypeEdited(QString const& text);

    //! Called when the return value description has been edited.
    void returnValueDescriptionEdited(QString const& text);


signals:
    //! Emitted when contents of the editor changes.
    void contentChanged();

private:
    // Disable copying.
    ApiFunctionEditor(ApiFunctionEditor const& rhs);
    ApiFunctionEditor& operator=(ApiFunctionEditor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list view for API functions.
    EditableListView functionList_;

    //! The model for the API functions.
    ApiFunctionModel functionModel_;

    //! Line edit for the function description.
    QLineEdit descEdit_;

    //! Combo box for the return value type.
    QComboBox returnTypeCombo_;

    //! Line edit for the return value description.
    QLineEdit returnValueDesc_;

    //! The table view widget for API function parameters.
    EditableTableView paramView_;

    //! The parameter delegate.
    ApiFunctionParameterDelegate paramDelegate_;

    //! The model to edit the parameters.
    ApiFunctionParameterModel paramModel_;

    //! The currently edited API function.
    QSharedPointer<ApiFunction> curFunc_;
};

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

#endif // APIFUNCTIONEDITOR_H
