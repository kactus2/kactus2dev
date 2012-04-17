//-----------------------------------------------------------------------------
// File: ComPropertyEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.4.2012
//
// Description:
// Editor for COM properties.
//-----------------------------------------------------------------------------

#ifndef COMPROPERTYEDITOR_H
#define COMPROPERTYEDITOR_H

#include "ComPropertyModel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
//! Property value editor.
//-----------------------------------------------------------------------------
class ComPropertyEditor : public QGroupBox
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    ComPropertyEditor(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ComPropertyEditor();

    /*!
     *  Sets the properties for editing.
     *
     *      @param [in] properties The properties.
     */
    void setProperties(QList< QSharedPointer<ComProperty> > const& properties);

    /*!
     *  Returns the edited properties.
     */
    QList< QSharedPointer<ComProperty> > const& getProperties() const;

signals:
    //! Emitted when contents of the editor changes.
    void contentChanged();

private:
    // Disable copying.
    ComPropertyEditor(ComPropertyEditor const& rhs);
    ComPropertyEditor& operator=(ComPropertyEditor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The table view widget.
    EditableTableView view_;

    //! \brief The filter to do the sorting of items to display.
    QSortFilterProxyModel filter_;

    //! \brief The model to edit the properties.
    ComPropertyModel model_;
};

//-----------------------------------------------------------------------------

#endif // COMPROPERTYEDITOR_H
