/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementeditor.h
 *		Project: Kactus 2
 */

#ifndef CONFIGURABLEELEMENTEDITOR_H
#define CONFIGURABLEELEMENTEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include "ConfigurableElementsModel.h"

#include <QAbstractItemModel>
#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QPushButton>

class ComponentItem;
class ExpressionParser;
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
	 *      @param [in] expressionParser        The parser to use for solving expressions.
	 *      @param [in] completionModel         The completion model for selecting parameter references.
	 *      @param [in] parent                  The parent widget.
	 *
	 *      @return <Description>.
	 */
	ConfigurableElementEditor(QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QAbstractItemModel* completionModel,
        QWidget *parent);
	
	//! The destructor
	virtual ~ConfigurableElementEditor();

	/*! Set the component instance to be edited.
	 *
	 * \param component Pointer to the component instance to edit.
	 *
	*/
	void setComponent(ComponentItem* component);

	/*! Clear the editor from all data.
	 *
	*/
	void clear();

signals:

	//! Emitted when contents of the editor changes.
	void contentChanged();

private slots:

	//! Handler for remove button clicks.
	void onRemoveClick();

private:
	//! No copying
	ConfigurableElementEditor(const ConfigurableElementEditor& other);

	//! No assignment
	ConfigurableElementEditor& operator=(const ConfigurableElementEditor& other);

	//! The widget to display the contents of the model.
	EditableTableView view_;

	//! The filter to do the sorting of items to display.
	QSortFilterProxyModel filter_;

	//! The model to edit the configurable elements of a component instance.
	ConfigurableElementsModel model_;
};

#endif // CONFIGURABLEELEMENTEDITOR_H
