//-----------------------------------------------------------------------------
// File: configurableelementeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 12.08.2011
//
// Description:
// Editor for configurable elements of a component instance.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLEELEMENTEDITOR_H
#define CONFIGURABLEELEMENTEDITOR_H

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <KactusAPI/include/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ConfigurableElementFinder.h>

#include <editors/common/ComponentInstanceEditor/ComponentInstanceConfigurableElementsModel.h>
#include <editors/common/ComponentInstanceEditor/configurableelementdelegate.h>
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsView.h>

#include <QAbstractItemModel>
#include <QCompleter>
#include <QGroupBox>
#include <QSharedPointer>
#include <QCheckBox>
#include <QSortFilterProxyModel>

class ExpressionParser;
class ParameterFinder;
class Component;
class ComponentInstance;
//-----------------------------------------------------------------------------
//! Editor for configurable elements of a component instance.
//-----------------------------------------------------------------------------
class ConfigurableElementEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterFinder                 The finder for configurable elements and top parameters.
	 *      @param [in] configurableElementFormatter    Formats referencing expressions in configurable elements.
	 *      @param [in] completionModel                 The completion model for selecting parameter references.
	 *      @param [in] parent                          The parent widget.
	 */
    ConfigurableElementEditor(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> configurableElementFormatter, QAbstractItemModel* completionModel,
        QWidget *parent);
	
	//! The destructor.
	virtual ~ConfigurableElementEditor() = default;

    //! No copying.
    ConfigurableElementEditor(const ConfigurableElementEditor& other) = delete;

    //! No assignment.
    ConfigurableElementEditor& operator=(const ConfigurableElementEditor& other) = delete;

	/*!
     *  Clear the editor from all data.
	 */
	virtual void clear() = 0;

signals:

	//! Emitted when contents of the editor changes.
	void contentChanged();

    /*!
     *  Increase the amount of references to a parameter corresponding to the ID.
     *
     *      @param [in] id      The ID of the parameter being searched for.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to a parameter corresponding to the ID.
     *
     *      @param [in] id      The ID of the parameter being searched for.
     */
    void decreaseReferences(QString id);

protected:

    /*!
     *  Set the model and filter for the view.
     *
     *      @param [in] newModel    The new model.
     *      @param [in] newFilter   The new filter.
     */
    void setModel(QAbstractItemModel* newModel, QSortFilterProxyModel* newFilter);

    /*!
     *  Hide the columns that should not be seen in the configurable elements editor.
     */
    void hideUnnecessaryColumns();

    //! The widget to display the contents of the model.
    ConfigurableElementsView view_;

    //! The delegate used in the display widget.
    ConfigurableElementDelegate* delegate_;

    //! The immediate configurable element values filter check box.
    QCheckBox* filterSelection_;



};

#endif // CONFIGURABLEELEMENTEDITOR_H
