//-----------------------------------------------------------------------------
// File: PropertyValueEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.4.2012
//
// Description:
// Editor for property values.
//-----------------------------------------------------------------------------

#ifndef PROPERTYVALUEEDITOR_H
#define PROPERTYVALUEEDITOR_H

#include "PropertyValueModel.h"

#include "PropertyValueDelegate.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
//! Property value editor.
//-----------------------------------------------------------------------------
class PropertyValueEditor : public QGroupBox
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    PropertyValueEditor(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~PropertyValueEditor();

    /*!
     *  Sets the allowed properties.
     *
     *      @param [in] properties The list of allowed properties.
     */
    void setAllowedProperties(QList< QSharedPointer<ComProperty> > const* properties);

    /*!
     *  Sets the data for editing.
     *
     *      @param [in] propertyValue The property values.
     */
    void setData(QMap<QString, QString> const& propertyValues);

    /*!
     *  Returns the edited data.
     */
    QMap<QString, QString> getData() const;

signals:
    //! Emitted when contents of the editor changes.
    void contentChanged();

private:
    // Disable copying.
    PropertyValueEditor(PropertyValueEditor const& rhs);
    PropertyValueEditor& operator=(PropertyValueEditor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The table view widget.
    EditableTableView view_;

    //! The item delegate.
    PropertyValueDelegate delegate_;

    //! \brief The filter to do the sorting of items to display.
    QSortFilterProxyModel filter_;

    //! \brief The model to edit the configurable elements of a component instance.
    PropertyValueModel model_;
};

//-----------------------------------------------------------------------------

#endif // PROPERTYVALUEEDITOR_H
