//-----------------------------------------------------------------------------
// File: EnumerationEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.01.2023
//
// Description:
// Editor for enumerations.
//-----------------------------------------------------------------------------

#ifndef ENUMERATIONEDITOR_H
#define ENUMERATIONEDITOR_H

#include <QFrame>
#include <QCheckBox>

class EnumCollectionEditor;

//-----------------------------------------------------------------------------
//! Editor for enumerations.
//-----------------------------------------------------------------------------
class EnumerationEditor : public QFrame
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] hideCheckAll    Flag for hiding the check all enumerations check box.
     *      @param [in] parent          The parent widget.
     */
    EnumerationEditor(bool hideCheckAll, QWidget* parent);

    /*!
     *  Destructor.
     */
    virtual ~EnumerationEditor() = default;

    /*!
     *  Returns the selected items.
     */
    QStringList getSelectedItems() const;

    /*!
     *  Setup the list of selected enumerations.
     *
     *      @param [in] availableItems  List of the available enumerations.
     *      @param [in] exclusiveItems  List of exclusive enumerations.
     *      @param [in] selectedItems   List of the selected enumerations.
     */
    void setupItems(QStringList const& availableItems, QStringList const& exclusiveItems, QStringList const& selectedItems);

signals:

    /*!
     *  Signal for canceling the editing.
     */
    void cancelEditing();

    /*!
     *  Signal for accepting the edits.
     */
    void finishEditing();

private slots:

    /*!
     *  Change the state of all the enumerations.
     *
     *      @param [in] newState    The new state for the enumerations.
     */
    void onCheckAllStateChange(bool newState);

    /*!
     *  Change the state of the check all button to match the state of the enumerations.
     *
     *      @param [in] newState    The new state.
     */
    void onChangeCheckAllState(Qt::CheckState newState);

private:
    // Disable copying.
    EnumerationEditor(EnumerationEditor const& rhs);
    EnumerationEditor& operator=(EnumerationEditor const& rhs);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The editor for enumerations.
    EnumCollectionEditor* enumCollectionEditor_;

    //! Check box for selecting / deselecting all enumerations.
    QCheckBox* selectAllCheck_;
};

#endif // ENUMERATIONEDITOR_H
