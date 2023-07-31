//-----------------------------------------------------------------------------
// File: ChoicesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.11.2014
//
// Description:
// Editor for component choices.
//-----------------------------------------------------------------------------

#ifndef CHOICESEDITOR_H
#define CHOICESEDITOR_H

#include <common/views/EditableListView/editablelistview.h>

#include <editors/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class EditableTableView;
class ChoicesModel;
class Component;
class ChoiceValidator;
class EnumerationModel;

//-----------------------------------------------------------------------------
//! Editor for component choices.
//-----------------------------------------------------------------------------
class ChoicesEditor : public ItemEditor
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] component   The component whose choices to edit.
     *      @param [in] validator   The validator used for choices.
     *      @param [in] parent      The parent widget.
     *
     *      @return <Description>.
     */
    ChoicesEditor(QSharedPointer<Component> component, QSharedPointer<ChoiceValidator> validator,
        QWidget* parent = 0);

    /*!
     *	The destructor
     */
    virtual ~ChoicesEditor() = default;

   /*! Check for the validity of the choices.
    * 
    *       @return True, if all the choices are valid, otherwise false.
    */
    virtual bool isValid() const;
    
    /*!
     *	Reloads the information from the model to the editor.
     */
    virtual void refresh();

public slots:

    /*!
     *	Called when the choise list is emptied or is no longer empty.
     *  
     *       @param [in] isEmpty     Indicates if the list was emptied or no longer empty.
     */
    void onChoiceListEmpty(bool isEmpty);

protected:

    /*!
     *	Called when the editor is shown.
     */
    void showEvent( QShowEvent* event );

private slots:

    /*!
     *	Called when a choice has been selected.
     * 
     *      @param [in] newIndex    Index of the selected choice.
     */
    void selectionChoiceChanged(QModelIndex const& newIndex);

private:

    //! No copying
    ChoicesEditor(const ChoicesEditor& other);
    ChoicesEditor& operator=(const ChoicesEditor& other);

    //! Creates the editor layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list view for choices.
    EditableListView choiceList_;

    //! Model for the component choices.
    ChoicesModel* choiceModel_;

    //! View for the choice enumerations.
    EditableTableView* enumerationView_;

    //! Model for choice enumerations.
    EnumerationModel* enumerationModel_;
};

#endif // CHOICESEDITOR_H
