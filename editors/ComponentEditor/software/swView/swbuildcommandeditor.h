//-----------------------------------------------------------------------------
// File: swbuildcommandeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 02.04.2013
//
// Description:
// Editor to set the SW build commands for SW view of a component.
//-----------------------------------------------------------------------------

#ifndef SWBUILDCOMMANDEDITOR_H
#define SWBUILDCOMMANDEDITOR_H

#include <QSharedPointer>
#include <QGroupBox>
#include <QList>
#include <QSortFilterProxyModel>

class Component;
class SWFileBuilder;
class EditableTableView;
class SWBuildCommandModel;

//-----------------------------------------------------------------------------
//! Editor to set the SW build commands for SW view of a component.
//-----------------------------------------------------------------------------
class SWBuildCommandEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] component           The component being edited.
	 *      @param [in] swBuildCommands     The sw file build commands to edit.
	 *      @param [in] parent              The owner of the editor.
	*/
	SWBuildCommandEditor(QSharedPointer<Component> component,
		QSharedPointer<QList<QSharedPointer<SWFileBuilder> > > swBuildCommands,
		QWidget *parent);
	
	//! The destructor.
	virtual ~SWBuildCommandEditor();

	/*! Check for the validity of the edited SW file build commands.
	*
	*      @return True if all build commands are in valid state.
	*/
	virtual bool isValid() const;

public slots:

	//! Refresh the contents to display.
	void refresh();

signals:

	//! Inform that contents of this editor have changed.
	void contentChanged();

private:
	
	//! No copying
	SWBuildCommandEditor(const SWBuildCommandEditor& other);

	//! No assignment
	SWBuildCommandEditor& operator=(const SWBuildCommandEditor& other);

	//! The view displaying the SW build commands.
	EditableTableView* view_;

	//! The proxy which sorts the items to display.
	QSortFilterProxyModel* proxy_;

	//! The model that manages the items.
	SWBuildCommandModel* model_;

	//! The component being edited.
	QSharedPointer<Component> component_;
};

#endif // SWBUILDCOMMANDEDITOR_H
