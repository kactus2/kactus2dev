//-----------------------------------------------------------------------------
// File: AddressEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.08.2012
//
// Description:
// Dockable address editor for HW designs.
//-----------------------------------------------------------------------------

#ifndef ADDRESSEDITOR_H
#define ADDRESSEDITOR_H

#include "AddressModel.h"

#include <models/component.h>

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSharedPointer>
#include <QTableView>
#include <QList>
#include <QSortFilterProxyModel>

class ComponentItem;
class GenericEditProvider;
class AddressTableView;

//-----------------------------------------------------------------------------
//! Dockable address editor for HW designs.
//-----------------------------------------------------------------------------
class AddressEditor : public QWidget
{
	Q_OBJECT

public:
	/*!
     *  Constructor.
	 *
	 *      @param [in] parent  The parent widget.
	 */
	AddressEditor(QWidget *parent);
	
	/*!
     *  Destructor.
     */
	virtual ~AddressEditor();

    /*!
     *  Sets the component to be edited.
	 *
	 *      @param [in] component The component to be edited.
	 */
	void setComponent(ComponentItem* component);

	
public slots:
    /*!
     *  Clears the contents of this editor.
	 */
	void clear();

	//! Handler for design widget's refreshed signal.
	void onRefresh();

	//! Set the editor to locked/unlocked mode.
	void setLocked(bool locked);

    //! Handles CSV import.
    void onImport();

    //! Handles CSV export.
    void onExport();

signals:
	//! Emitted when the editor changes some value.
	void contentChanged();

private:
    // Disable copying.
    AddressEditor(AddressEditor const& rhs);
    AddressEditor& operator=(AddressEditor const& rhs);

	/*! 
     *  Sets up the layout of this widget.
     */
	void setupLayout();

	/*!
     *  Set up the signals between widgets.
     */
	void setupConnections();
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component being edited.
    ComponentItem* component_;

    //! The edit provider.
    GenericEditProvider* editProvider_;

    //! Button for address auto-assignment.
    QPushButton* autoAssignButton_;

    //! Button for CSV import.
    QPushButton* csvImportButton_;

    //! Button for CSV export.
    QPushButton* csvExportButton_;

    //! The address model.
    AddressModel model_;

    //! \brief The widget to display the contents of the model.
    AddressTableView* view_;

    //! \brief The filter to do the sorting of items to display.
    QSortFilterProxyModel* filter_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSEDITOR_H
