//-----------------------------------------------------------------------------
// File: Component.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

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
class AddressBlockValidator;
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
	 *      @param [in] component               The component being edited.
	 *      @param [in] handler                 The instance managing the library.
	 *      @param [in] memoryRemap             The memory remap being edited.
	 *      @param [in] parameterFinder         The parameter finder for component.
	 *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] expressionParser        The expression parser.
     *      @param [in] addressBlockValidator   Validator used for address blocks.
     *      @param [in] addressUnitBits         The current address unit bits.
	 *      @param [in] parent                  The parent of this editor.
	 */
	MemoryMapEditor(QSharedPointer<Component> component, LibraryInterface* handler, 
        QSharedPointer<MemoryMapBase> memoryRemap,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<AddressBlockValidator> addressBlockValidator,
        QString const& addressUnitBits,
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
    void graphicsChanged();

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

private:
	
	//! No copying.
	MemoryMapEditor(const MemoryMapEditor& other);

	//! No assignment.
	MemoryMapEditor& operator=(const MemoryMapEditor& other);

	//! The view to show the details of a memory map.
	EditableTableView* view_;

	//! The model that manages the items.
	MemoryMapModel* model_;
};

#endif // MEMORYMAPEDITOR_H
