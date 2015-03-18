/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementeditor.h
 *		Project: Kactus 2
 */

#ifndef CONFIGURABLEELEMENTEDITOR_H
#define CONFIGURABLEELEMENTEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include "ConfigurableElementsModel.h"

#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QPushButton>

class ComponentItem;
class ParameterFinder;
//-----------------------------------------------------------------------------
//! Editor for configurable elements of a component instance.
//-----------------------------------------------------------------------------
class ConfigurableElementEditor : public QGroupBox {
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterFinder         The pointer to the instance for finding parameters.
	 *      @param [in] expressionFormatter     The pointer to the instance for formatting referencing expressions.
	 *      @param [in] parent                  The pointer to the owner of this editor.
	 *      @param [in] title                   Contains the text to be set as the title of the group box.
	 */
	ConfigurableElementEditor(QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QWidget *parent, 
		const QString& title = tr("Configurable element values"));
	
	//! \brief The destructor
	virtual ~ConfigurableElementEditor();

	/*! \brief Set the component instance to be edited.
	 *
	 * \param component Pointer to the component instance to edit.
	 *
	*/
	void setComponent(ComponentItem* component);

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

	//! \brief The widget to display the contents of the model.
	EditableTableView view_;

	//! \brief The filter to do the sorting of items to display.
	QSortFilterProxyModel filter_;

    //! Model for all the parameters within a single component.
    ComponentParameterModel* componentParameterModel_;

	//! \brief The model to edit the configurable elements of a component instance.
	ConfigurableElementsModel model_;
};

#endif // CONFIGURABLEELEMENTEDITOR_H
