//-----------------------------------------------------------------------------
// File: EnumCollectionEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 17.04.2013
//
// Description:
// Editor for choosing multiple values from a collection of enumerables.
//-----------------------------------------------------------------------------

#ifndef ENUMCOLLECTIONEDITOR_H
#define ENUMCOLLECTIONEDITOR_H

#include <QFrame>
#include <QVBoxLayout>
#include <QCheckBox>

//-----------------------------------------------------------------------------
//! Editor for choosing multiple values from a collection of enumerables.
//-----------------------------------------------------------------------------
class EnumCollectionEditor : public QWidget
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    EnumCollectionEditor(QWidget* parent);

    /*!
     *  Destructor.
     */
    virtual ~EnumCollectionEditor() = default;

    /*!
     *  Adds a new enumerable item to the editor.
     *
     *      @param [in] text                The name of the item.
     *      @param [in] isExclusivePort     Flag for exclusive ports.
     *      @param [in] selected            If true, the item is initially selected.
     */
    void addItem(QString const& text, bool isExclusivePort, bool selected);

    void clearItems();

    /*!
     *  Returns the selected items.
     */
    QStringList getSelectedItems() const;

    /*!
     *  Change the state of all the enumerations.
     *
     *      @param [in] newState    New state for the enumerations.
     */
    void changeStatesOfAllItems(bool newState);

signals:

    /*!
     *  Inform of a change in the combined state of the enumerations.
     *
     *      @param [in] newState    Combined state of the enumerations.
     */
    void itemStateChanged(Qt::CheckState newState);

protected:

    /*!
     *  Repositions the editor higher, if it would otherwise exceed the bottom of parent widget.
     */
    virtual void rePosition();

private slots:

    /*!
     *  Handle the state change in enumeration check box.
     *
     *      @param [in] newState    New state of the enumeration check box.
     */
    void onItemClicked(bool newState);

    /*!
     *  Handle the state change in an exclusive enumeration check box.
     *
     *      @param [in] newState    New state of the exclusive enumeration check box.
     */
    void onExclusiveItemClicked(bool newState);

private:
    // Disable copying.
    EnumCollectionEditor(EnumCollectionEditor const& rhs);
    EnumCollectionEditor& operator=(EnumCollectionEditor const& rhs);

    /*!
     *  Create an enumeration check box.
     *
     *      @param [in] text        The name of the item.
     *      @param [in] selected    If true, the item is initially selected.
     *
     *      @return The created enumeration check box.
     */
    QCheckBox* createCheckBox(QString const& text, bool isSelected);

    /*!
     *  Handle the change in the state of the check all check box.
     *
     *      @param [in] newState    New state of the item initiating this change.
     */
    void handleCheckAllStateChangeFromItemClick(bool newState);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The item layout.
    QVBoxLayout* layout_;

    //! The check boxes for the items.
    QList<QCheckBox*> items_;

    //! List of exclusive check boxes.
    QList<QCheckBox*> exclusiveItems_;
};

#endif // ENUMCOLLECTIONEDITOR_H
