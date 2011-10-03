/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementeditor.h
 *		Project: Kactus 2
 */

#ifndef CONFIGURABLEELEMENTEDITOR_H
#define CONFIGURABLEELEMENTEDITOR_H

#include "componentinstancemodel.h"

#include <QGroupBox>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QPushButton>

class DiagramComponent;

/*! \brief Editor for configurable elements of a component instance.
 *
 */
class ConfigurableElementEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of this editor.
	 * \param title Contains the text to be set as the title of the group box.
	 *
	*/
	ConfigurableElementEditor(QWidget *parent, 
		const QString& title = tr("Configurable element values"));
	
	//! \brief The destructor
	virtual ~ConfigurableElementEditor();

	/*! \brief Set the component instance to be edited.
	 *
	 * \param component Pointer to the component instance to edit.
	 *
	*/
	void setComponent(DiagramComponent* component);

	/*! \brief Clear the editor from all data.
	 *
	*/
	void clear();

signals:

	//! \brief Emitted when contents of the editor changes.
	void contentChanged();

private slots:

	//! \brief Handler for remove button clicks.
	void onRemoveClick();

private:
	//! \brief No copying
	ConfigurableElementEditor(const ConfigurableElementEditor& other);

	//! \brief No assignment
	ConfigurableElementEditor& operator=(const ConfigurableElementEditor& other);

	//! \brief Pointer to the component instance being edited.
	DiagramComponent* component_;

	//! \brief The widget to display the contents of the model.
	QTableView view_;

	//! \brief The filter to do the sorting of items to display.
	QSortFilterProxyModel filter_;

	//! \brief The model to edit the configurable elements of a component instance.
	ComponentInstanceModel model_;

	//! \brief The button to add a new configurable element value.
	QPushButton addButton_;

	//! \brief The button to remove the selected configurable element value.
	QPushButton removeButton_;
};

#endif // CONFIGURABLEELEMENTEDITOR_H
