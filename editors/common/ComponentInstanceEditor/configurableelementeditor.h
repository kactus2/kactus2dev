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

#include <editors/common/ExpressionSet.h>
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsModel.h>
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsFilter.h>
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
     *      @param [in] parameterExpressions            The expressions for selecting parameter references.
     *      @param [in] defaultExpressions              The expressions for resolving default values.
	 *      @param [in] completionModel                 The completion model for selecting parameter references.
	 *      @param [in] parent                          The parent widget.
	 */
    ConfigurableElementEditor(
        ExpressionSet parameterExpressions,
        ExpressionSet defaultExpressions,
        QAbstractItemModel* completionModel,
        QWidget *parent);
	
    void setParameters(QString const& containingItemName, 
        QSharedPointer<QList<QSharedPointer<Parameter> > > parameters, 
        QSharedPointer<QList<QSharedPointer<Choice> > > choices,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > storedConfigurableElements);

    //! The destructor.
	virtual ~ConfigurableElementEditor() = default;

    //! No copying.
    ConfigurableElementEditor(const ConfigurableElementEditor& other) = delete;

    //! No assignment.
    ConfigurableElementEditor& operator=(const ConfigurableElementEditor& other) = delete;

	/*!
     *  Clear the editor from all data.
	 */
    virtual void clear();;

    void setEditProvider(QSharedPointer<IEditProvider> editProvider);

signals:

	//! Emitted when contents of the editor changes.
	void contentChanged();

    /*!
     *  Increase the amount of references to a parameter corresponding to the ID.
     *
     *      @param [in] id      The ID of the parameter being searched for.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the amount of references to a parameter corresponding to the ID.
     *
     *      @param [in] id      The ID of the parameter being searched for.
     */
    void decreaseReferences(QString const& id);

private slots:

    void sendSignalForElementRemoval(QString const& elementID, int elementRow);

protected:

    /*!
     *  Hide the columns that should not be seen in the configurable elements editor.
     */
    void hideUnnecessaryColumns();

    //! The widget to display the contents of the model.
    ConfigurableElementsView view_;

    //! The delegate used in the display widget.
    ConfigurableElementDelegate* delegate_;

    ConfigurableElementsFilter* filter_;

    ConfigurableElementsModel* model_;

    //! The immediate configurable element values filter check box.
    QCheckBox* filterSelection_;

};

#endif // CONFIGURABLEELEMENTEDITOR_H
