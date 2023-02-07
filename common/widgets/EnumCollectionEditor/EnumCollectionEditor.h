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
    ~EnumCollectionEditor();

    /*!
     *  Adds a new enumerable item to the editor.
     *
     *      @param [in] name      The name of the item.
     *      @param [in] selected  If true, the item is initially selected.
     */
    void addItem(QString const& text, bool selected = false);

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

private:
    // Disable copying.
    EnumCollectionEditor(EnumCollectionEditor const& rhs);
    EnumCollectionEditor& operator=(EnumCollectionEditor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The item layout.
    QVBoxLayout* layout_;

    //! The check boxes for the items.
    QList<QCheckBox*> items_;
};

#endif // ENUMCOLLECTIONEDITOR_H
