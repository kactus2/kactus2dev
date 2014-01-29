//-----------------------------------------------------------------------------
// File: EnumCollectionEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
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
class EnumCollectionEditor : public QFrame
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
