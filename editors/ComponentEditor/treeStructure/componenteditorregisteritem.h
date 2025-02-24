//-----------------------------------------------------------------------------
// File: componenteditorregisteritem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// The item for single register in component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORREGISTERITEM_H
#define COMPONENTEDITORREGISTERITEM_H

#include "componenteditoritem.h"

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <editors/ComponentEditor/referenceCounter/ReferenceCounter.h>

#include <QSharedPointer>
#include <QList>

class MemoryMapsVisualizer;
class MemoryVisualizationItem;
class RegisterGraphItem;
class ExpressionParser;
class Component;
class Register;
class Field;
class RegisterValidator;
class FieldInterface;
class RegisterInterface;
class RegisterBase;

//-----------------------------------------------------------------------------
//! The item for single register in component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorRegisterItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] reg                     The register being edited.
     *    @param [in] containingRegisterData  Register data containing the edited register.
	 *    @param [in] model                   The model that owns the items.
	 *    @param [in] libHandler              The instance that manages the library.
	 *    @param [in] component               The component being edited.
	 *    @param [in] parameterFinder         The parameter finder.
	 *    @param [in] expressionFormatter     The expression formatter.
	 *    @param [in] referenceCounter        The instance counting references made to parameters.
	 *    @param [in] expressionParser        The expression parser to use.
     *    @param [in] registerValidator       Validator for registers.
     *    @param [in] registerInterface       Interface for registers.
	 *    @param [in] parent                  The parent item.
	 */
	ComponentEditorRegisterItem(QSharedPointer<Register> reg, 
        QSharedPointer<QList<QSharedPointer<RegisterBase> > > containingRegisterData,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<RegisterValidator> registerValidator,
        RegisterInterface* registerInterface,
		ComponentEditorItem* parent);

	/*!
     *  The destructor.
     */
	virtual ~ComponentEditorRegisterItem() = default;

    //! No copying. No assignment.
    ComponentEditorRegisterItem(const ComponentEditorRegisterItem& other) = delete;
    ComponentEditorRegisterItem& operator=(const ComponentEditorRegisterItem& other) = delete;

	/*! Get the tool tip for the item.
	 * 
	 *    @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*! Get the text to be displayed to user in the tree for this item.
	 *
	 *    @return QString Contains the text to display.
	 */
	virtual QString text() const;

	/*! Check the validity of this item and sub items.
	 *
	 *    @return bool True if item is in valid state.
	 */
	virtual bool isValid() const;

	/*! Get pointer to the editor of this item.
	 *
	 *    @return Pointer to the editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*! Add a new child to the item.
	 * 
	 *    @param [in] index The index to add the child into.
	 */
	virtual void createChild(int index) override final;

    /*! Remove a child from the item.
     *
     *    @param [in] index The index to remove the child from.
     */
    virtual void removeChild(int index) override final;

	/*! Get pointer to the visualizer of this item.
	 * 
	 *    @return Pointer to the visualizer to use for this item.
	 */
	virtual ItemVisualizer* visualizer() override final;

	/*! Set the visualizer for this item.
	 *
	 *    @param [in] visualizer Pointer to the visualizer.
	 */
    void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*! Get the visualizer graphics item for the register.
	 *
	 *    @return QGraphicsItem* Pointer to the graphics item.
	 */
	virtual QGraphicsItem* getGraphicsItem() override final;

    //! Update the graphics item of the register.
    virtual void updateGraphics() override final;

	//! Remove the graphics item of the register.
	virtual void removeGraphicsItem() override final;

signals:

    /*
     *  Informs of field name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void fieldNameChanged(QString const& oldName, QString const& newName);

    /*
     *  Informs of register name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void registerNameChanged(QString const& oldName, QString const& newName);

    //! Signals a change in the item's address data.
    void addressingChanged();



public slots:

    //! Handle the change in item's addressing data.
    void onAddressingChanged();

    //! Handle the change in child item's addressing data.
    void onChildAddressingChanged();

protected slots:

    /*!
     *  Handle the change in graphics.
     */
    virtual void onGraphicsChanged();

    //! Handle the change in child item's graphics.
    void onChildGraphicsChanged(int index);
   
private:
	

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The register being edited.
	QSharedPointer<Register> reg_;

	//! The visualizer of memory maps.
	MemoryMapsVisualizer* visualizer_ = nullptr;

	//! The graph item that visualizes the register and possible dimensions.
	RegisterGraphItem* registerItem_ = nullptr;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The used register validator.
    QSharedPointer<RegisterValidator> registerValidator_;

    //! Interface for registers.
    RegisterInterface* registerInterface_;

    //! Register data containing the edited register.
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > containingRegisterData_;
};

#endif // COMPONENTEDITORREGISTERITEM_H
