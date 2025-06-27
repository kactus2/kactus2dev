//-----------------------------------------------------------------------------
// File: componenteditorfielditem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 28.08.2012
//
// Description:
// The item for single field in component editor's navigation tree
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORFIELDITEM_H
#define COMPONENTEDITORFIELDITEM_H

#include "componenteditoritem.h"

#include <QFont>
#include <QSharedPointer>

class MemoryMapsVisualizer;
class FieldGraphItem;
class ExpressionParser;
class Field;
class Register;
class Component;
class FieldValidator;
class FieldInterface;
class ResetInterface;

//-----------------------------------------------------------------------------
//! The item for single field in component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorFieldItem : public ComponentEditorItem
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *    @param [in] reg                 The register which contains this field.
	 *    @param [in] field               The field being edited.
	 *    @param [in] model               The model that owns the items.
	 *    @param [in] libHandler          The instance that manages the library.
	 *    @param [in] component           The component being edited.
	 *    @param [in] parameterFinder     The parameter finder.
	 *    @param [in] referenceCounter    The instance for counting references made to the parameters.
     *    @param [in] expressionParser    Expression parser to use.
     *    @param [in] formatter           The expression formatter.
     *    @param [in] fieldValidator      Validator used for fields.
     *    @param [in] fieldInterface      Interface for accessing fields.
     *    @param [in] parent              The parent item.
	 */
    ComponentEditorFieldItem(QSharedPointer<Register> reg,
        QSharedPointer<Field> field,
        ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> formatter,
        QSharedPointer<FieldValidator> fieldValidator,
        FieldInterface* fieldInterface,
        ComponentEditorItem* parent);

	/*!
     *  The destructor.
     */
	virtual ~ComponentEditorFieldItem() = default;

    //! No copying.
    ComponentEditorFieldItem(const ComponentEditorFieldItem& other) = delete;

    //! No assignment.
    ComponentEditorFieldItem& operator=(const ComponentEditorFieldItem& other) = delete;


	/*!
     *  Get the tool tip for the item.
	 * 
	 *    @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *    @return QString Contains the text to display.
	 */
	virtual QString text() const;

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *    @return bool True if item is in valid state.
	 */
	virtual bool isValid() const;

	/*!
     *  Get the editor of this item.
	 *
	 *    @return The editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
     *  Get the visualizer of this item.
	 * 
	 *    @return The visualizer to use for this item.
	 */
	virtual ItemVisualizer* visualizer();

	/*!
     *  Set the visualizer for this item.
	 *
	 *    @param [in] visualizer The visualizer to set.
	 */
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*!
     *  Get the visualizer graphics item for the field.
	 *
	 *    @return The graphics item.
	 */
	virtual QGraphicsItem* getGraphicsItem();

	/*!
	 *  Get the visualizer graphics items for the field.
	 *
	 *    @return List of the graphics items.
	 */
	QList<FieldGraphItem*> const& getGraphicsItems() const;

	/*!
     *  Update the graphics item of the field.
	 */
    virtual void updateGraphics() override final;

	/*!
     *  Remove the graphics item of the field.
	 */
	virtual void removeGraphicsItem();

	/*!
	 *  Remove the graphics items of the field.
	 */
	virtual void removeGraphicsItems();

    /*!
     *	Create the graphics items of the field. Multiple items are created if the field has dimension.
     */
    void createGraphicsItems();

signals:

    /*!
     *  Inform of the need to redraw the visualization.
     */
    //void graphicsChanged();

    //! Inform that address information has changed.
    void addressingChanged();


    /*
     *  Informs of field name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void fieldNameChanged(QString const& oldName, QString const& newName);

protected slots:

	/*!
     *  Handler for editor's contentChanged signal.
	 */
	virtual void onEditorChanged();

    /*!
     *  Handle the change in graphics.
     */
    virtual void onGraphicsChanged() override final;

private:

	//! The register which contains this field.
	QSharedPointer<Register> reg_;

	//! The field being edited.
	QSharedPointer<Field> field_;

	//! The visualizer of memory maps.
	MemoryMapsVisualizer* visualizer_ = nullptr;

	//! The graph item that visualizes the field. 
    FieldGraphItem* graphItem_ = nullptr;               // TODO remove this and use only graphItems_

	//! The graph items representing the field and its possible replicas.
	QList<FieldGraphItem* > graphItems_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The used field validator.
    QSharedPointer<FieldValidator> fieldValidator_;

    //! Interface for fields.
    FieldInterface* fieldInterface_;
};

#endif // COMPONENTEDITORFIELDITEM_H
