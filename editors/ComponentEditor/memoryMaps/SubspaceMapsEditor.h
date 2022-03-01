//-----------------------------------------------------------------------------
// File: SubspaceMapsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.01.2022
//
// Description:
// The editor to edit the subspace maps of a single memory map.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAPEDITOR_H
#define SUBSPACEMAPEDITOR_H

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <IPXACTmodels/Component/MemoryMapBase.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>
#include <QGroupBox>

class EditableTableView;
class LibraryInterface;
class ExpressionParser;
class Component;
class SubspaceMapModel;
class SubspaceMapInterface;

//-----------------------------------------------------------------------------
//! The editor to edit the subspace maps of a single memory map.
//-----------------------------------------------------------------------------
class SubspaceMapsEditor : public QGroupBox
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] component               The component being edited.
	 *      @param [in] parameterFinder         The parameter finder for component.
     *      @param [in] expressionParser        The expression parser.
     *      @param [in] subspaceMapInterface    Interface for accessing subspace maps.
     *      @param [in] blocks                  Pointer to the available memory blocks.
	 *      @param [in] parent                  The parent of this editor.
	 */
    SubspaceMapsEditor(QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        SubspaceMapInterface* subspaceMapInterface,
        QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > blocks,
        QWidget* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~SubspaceMapsEditor() = default;

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
    void subspaceMapNameChanged(QString const& oldName, QString const& newName);

    /*!
     *  Inform the other memory block editor to invalidate the filter.
     */
    void invalidateOtherFilter();

    /*!
     *  Inform this editor to invalidate the filter.
     */
    void invalidateThisFilter();
    
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

private:
	
    //! No copying.	No assignment.
    SubspaceMapsEditor(const SubspaceMapsEditor& other);
    SubspaceMapsEditor& operator=(const SubspaceMapsEditor& other);

	//! The view to show the details of a memory map.
	EditableTableView* view_;

	//! The model that manages the items.
    SubspaceMapModel* model_;

    //! Interface for accessing address blocks.
    SubspaceMapInterface* interface_;

    //! Pointer to the available address blocks.
    QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > blocks_;

    //! The containing component.
    QSharedPointer<Component> component_;
};

#endif // SUBSPACEMAPEDITOR_H
