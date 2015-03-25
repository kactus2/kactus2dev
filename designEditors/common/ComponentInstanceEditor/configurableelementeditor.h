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
#include <editors/ComponentEditor/common/ListParameterFinder.h>

#include "ConfigurableElementsModel.h"
#include "configurableelementdelegate.h"

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
	 *      @param [in] listFinder                      The finder for the configurable element values.
	 *      @param [in] parameterFinder                 The finder for configurable elements and top parameters.
	 *      @param [in] configurableElementFormatter    Formats referencing expressions in configurable elements.
	 *      @param [in] instanceExpressionFormatter     Formats referencing expressions in component instance.
	 *      @param [in] expressionParser                Solves expressions in configurable elements.
	 *      @param [in] instanceParser                  Solves expressions in default values (component instance).
	 *      @param [in] completionModel                 The completion model for selecting parameter references.
	 *      @param [in] parent                          The parent widget.
	 */
	ConfigurableElementEditor(QSharedPointer<ListParameterFinder> listFinder,
        QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionFormatter> configurableElementFormatter,
        QSharedPointer<ExpressionFormatter> instanceExpressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionParser> instanceParser,
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

    //! The delegate used in the display widget.
    QSharedPointer<ConfigurableElementDelegate> delegate_;
};

#endif // CONFIGURABLEELEMENTEDITOR_H
