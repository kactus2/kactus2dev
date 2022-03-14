//-----------------------------------------------------------------------------
// File: Component.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.08.2012
//
// Description:
// The editor to edit the address blocks of a single memory map.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPEDITOR_H
#define MEMORYMAPEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <IPXACTmodels/Component/MemoryMapBase.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>
#include <QGroupBox>

class EditableTableView;
class MemoryMapModel;
class MemoryMapProxy;
class LibraryInterface;
class ExpressionParser;
class Component;
class AddressBlockInterface;
class AddressBlock;
class MemoryBlockFilter;

//-----------------------------------------------------------------------------
//! The editor to edit the address blocks of a single memory map.
//-----------------------------------------------------------------------------
class MemoryMapEditor : public QGroupBox
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] component           The component being edited.
	 *      @param [in] handler             The instance managing the library.
	 *      @param [in] parameterFinder     The parameter finder for component.
     *      @param [in] expressionParser    The expression parser.
     *      @param [in] blockInterface      Interface for address blocks.
     *      @param [in] blocks              Pointer to the available address blocks.
	 *      @param [in] parent              The parent of this editor.
	 */
    MemoryMapEditor(QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        AddressBlockInterface* blockInterface,
        QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > blocks,
        QWidget* parent = 0);

	//! The destructor.
	virtual ~MemoryMapEditor();

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:
    
    /*!
     *  Emitted when the contents of the widget change.
     */
    void contentChanged();

    /*!
     *  Informs of a need to redraw the visualization.
     */
    void graphicsChanged(int);

    void childAddressingChanged(int);

    /*!
     *  Emits an error message for the user.
     *
     *      @param [in] msg     The error message.
     */
    void errorMessage(const QString& msg) const;

    /*!
     *  Emits a notice message for the user.
     *
     *      @param [in] msg   [Description].
     */
    void noticeMessage(const QString& msg) const;

    /*!
     *  Increases the number of references to the given parameter.
     *
     *      @param [in] id  The target parameter.
     */
    void increaseReferences(QString id);

    /*!
     *  Decreases the number of references to the given parameter.
     *
     *      @param [in] id  The target parameter.
     */
    void decreaseReferences(QString id);

    /*!
     *  Informs of an added child.
     *
     *      @param [in] index   The index of the new child.
     */
    void childAdded(int index);

    /*!
     *  Informs of a removed child.
     *
     *      @param [in] index   The index of the removed child.
     */
    void childRemoved(int index);

    /*!
     *  Assign new address unit bits for address blocks.
     *
     *      @param [in] newAddressUnitBits  The new address unit bits.
     */
    void assignNewAddressUnitBits(QString const& newAddressUnitBits);

    /*
     *  Informs of address block name change.
     *
     *      @param [in] oldName     The old name.
     *      @param [in] newName     The new name.
     */
    void addressBlockNameChanged(QString const& oldName, QString const& newName);

    /*!
     *  Inform the other memory block editor to invalidate the filter.
     */
    void invalidateOtherFilter();

    /*!
     *  Inform this editor to invalidate the filter.
     */
    void invalidateThisFilter();


private:
	
    //! No copying.	No assignment.
	MemoryMapEditor(const MemoryMapEditor& other);
	MemoryMapEditor& operator=(const MemoryMapEditor& other);

	//! The view to show the details of a memory map.
	EditableTableView* view_;

	//! The model that manages the items.
	MemoryMapModel* model_;

    //! Interface for accessing address blocks.
    AddressBlockInterface* interface_;

    //! Pointer to the available address blocks.
    QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > blocks_;

    //! The containing component.
    QSharedPointer<Component> component_;

    //! Filter fro address blocks.
    MemoryBlockFilter* proxy_;
};

#endif // MEMORYMAPEDITOR_H
