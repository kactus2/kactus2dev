//-----------------------------------------------------------------------------
// File: SubspaceMapItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.02.2022
//
// Description:
// The item for a single subspace map in component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAPITEM_H
#define SUBSPACEMAPITEM_H

#include <editors/ComponentEditor/treeStructure/componenteditoritem.h>

#include <IPXACTmodels/Component/SubSpaceMap.h>

#include <QFont>
#include <QSharedPointer>

class MemoryMapsVisualizer;
class MemoryMapBase;
class SubspaceMapValidator;
class SubspaceMapInterface;

//-----------------------------------------------------------------------------
//! The item for a single subspace map in component editor's navigation tree.
//-----------------------------------------------------------------------------
class SubspaceMapItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] containingMap           Memory map containing the address block.
	 *      @param [in] subspace                The subspace map being edited.
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The instance for counting references to parameters.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
	 *      @param [in] expressionParser        The expression formatter.
     *      @param [in] addressBlockValidator   Validator used for address blocks.
     *      @param [in] subspaceInterface       Interface for accessing subspace maps.
	 *      @param [in] parent                  The parent item.
	 */
    SubspaceMapItem(QSharedPointer<MemoryMapBase> containingMap,
        QSharedPointer<SubSpaceMap> subspace,
        ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<SubspaceMapValidator> subspaceValidator,
        SubspaceMapInterface* subspaceInterface,
		ComponentEditorItem* parent);

	/*!
     *  The destructor.
     */
    virtual ~SubspaceMapItem() = default;

    //! No copying. No assignment.
    SubspaceMapItem(const SubspaceMapItem& other) = delete;
    SubspaceMapItem& operator=(const SubspaceMapItem& other) = delete;

	/*!
     *  Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const override final;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	 */
	virtual QString text() const override final;

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	 */
	virtual bool isValid() const override final;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *      @return The editor to use for this item.
	 */
	virtual ItemEditor* editor() override final;

    /*!
     *  Get pointer to the visualizer of this item.
	 * 
	 *      @return The visualizer to use for this item.
	 */
	virtual ItemVisualizer* visualizer() override final;

	/*!
     *  Set the visualizer for this item.
	 *
	 *      @param [in] visualizer The visualizer.
	 */
	void setVisualizer(MemoryMapsVisualizer* visualizer);

	/*!
     *  Update the graphics item of the address block.
	 */
	virtual void updateGraphics() override final;

	/*!
     *  Remove the graphics item of the address block.
	 */
	virtual void removeGraphicsItem() override final;

    /*!
     *  Change the address unit bits in component editor.
     *
     *      @param [in] newAddressUnitBits   The new value for address unit bits.
     */
    void addressUnitBitsChanged(int newAddressUnitBits);

public slots:

    /*!
     *  Handle the change in item's addressing data.
     */
    void onAddressingChanged();

protected slots:
    
    /*!
     *  Handles the redrawing of the visualization of the item.
     */
    virtual void onGraphicsChanged();

signals:
    
    /*!
     *  Signals a change in the item's address data.
     */
    void addressingChanged();

    /*
     *  Informs of subspace map name change.
     *
     *      @param [in] oldName     The old name.
     *      @param [in] newName     The new name.
     */
    void subspaceNameChanged(QString const& oldName, QString const& newName);

private:
	
    //! Memory map containing the edited address block.
    QSharedPointer<MemoryMapBase> containingMap_;

	//! The subspace map being edited.
	QSharedPointer<SubSpaceMap> subspaceMap_;

	//! The visualizer for memory maps.
	MemoryMapsVisualizer* visualizer_;

	//! The graph item which visualizes the address block.
// 	AddressBlockGraphItem* graphItem_;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The number of addressable unit bits in the address block.
    unsigned int addressUnitBits_;

    //! The used subspace map validator.
    QSharedPointer<SubspaceMapValidator> subspaceValidator_;

    //! Interface for accessing subspace maps.
    SubspaceMapInterface* subspaceInterface_;
};

#endif // SUBSPACEMAPITEM_H
