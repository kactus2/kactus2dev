/* 
 *	Created on:	2.4.2013
 *	Author:		Antti Kamppi
 *	File name:	swbuildcommandeditor.h
 *	Project:		Kactus 2
*/

#ifndef SWBUILDCOMMANDEDITOR_H
#define SWBUILDCOMMANDEDITOR_H

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/swbuildcommand.h>

#include <QSharedPointer>
#include <QGroupBox>
#include <QList>
#include <QSortFilterProxyModel>

class EditableTableView;
class SWBuildCommandModel;

/*! \brief Editor to set the SW build commands for SW view of a component.
 *
 */
class SWBuildCommandEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		SWBuildCommandEditor
	 * Full name:	SWBuildCommandEditor::SWBuildCommandEditor
	 * Access:		private 
	 *
	 * \param component Pointer to the component being edited.
	 * \param swBuildCommands Contains the sw file build commands to edit.
	 * \param parent Pointer to the owner of the editor.
	 *
	*/
	SWBuildCommandEditor(QSharedPointer<Component> component,
		QList<QSharedPointer<SWBuildCommand> >& swBuildCommands,
		QWidget *parent);
	
	//! \brief The destructor.
	virtual ~SWBuildCommandEditor();

	/*! \brief Check for the validity of the edited SW file build commands.
	*
	* \return True if all build commands are in valid state.
	*/
	virtual bool isValid() const;

public slots:

	//! \brief Refresh the contents to display.
	void refresh();

signals:

	//! \brief Inform that contents of this editor have changed.
	void contentChanged();

private:
	
	//! \brief No copying
	SWBuildCommandEditor(const SWBuildCommandEditor& other);

	//! \brief No assignment
	SWBuildCommandEditor& operator=(const SWBuildCommandEditor& other);

	//! \brief The view displaying the SW build commands.
	EditableTableView* view_;

	//! \brief The proxy which sorts the items to display.
	QSortFilterProxyModel* proxy_;

	//! \brief The model that manages the items.
	SWBuildCommandModel* model_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // SWBUILDCOMMANDEDITOR_H
