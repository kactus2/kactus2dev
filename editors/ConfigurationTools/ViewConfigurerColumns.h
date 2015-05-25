//-----------------------------------------------------------------------------
// File: ViewConfigurerColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2015
//
// Description:
// Common declarations for view configurer.
//-----------------------------------------------------------------------------

#include <QObject>

namespace ViewConfigurerColumns
{
    //! The columns in the view configurer.
    enum columns
    {
        ITEM_VLNV = 0,
        INSTANCE_NAME,
        INSTANCE_VIEW,
        INSTANCE_ID,
        COLUMN_COUNT
    };

    //! Text for an item that does not contain a view.
    QString const NOVIEWTEXT = QObject::tr("No view selected");

    //! Text for an item that causes cyclical instantiation of component instances.
    QString const CYCLICCOMPONENTTEXT = QObject::tr("Cyclic instantiation");
}