//-----------------------------------------------------------------------------
// File: SubspaceMapDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.02.2022
//
// Description:
// The delegate that provides editors to add/remove/edit details of subspace maps.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAPDELEGATE_H
#define SUBSPACEMAPDELEGATE_H

#include <editors/ComponentEditor/memoryMaps/MemoryBlockDelegate.h>

#include <IPXACTmodels/common/Document.h>

class SubspaceMapInterface;
class BusInterfaceInterface;
class AddressSpace;

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to add/remove/edit details of subspace maps.
//-----------------------------------------------------------------------------
class SubspaceMapDelegate : public MemoryBlockDelegate
{
	Q_OBJECT

public:
	
	/*!
	 *  The constructor.
	 *
     *      @param [in] completionModel         Model containing the completions used in expression editor.
	 *      @param [in] parameterFinder         The parameter finder for expression editor.
     *      @param [in] subspaceInterface       Interface for accessing subspace maps.
     *      @param [in] docRevision             The IP-XACT standard revision to comply to.
	 *      @param [in] parent                  Pointer to the owner of the delegate.
	 */
	SubspaceMapDelegate(QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> parameterFinder, 
        SubspaceMapInterface* subspaceInterface, QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces,
        Document::Revision docRevision, QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~SubspaceMapDelegate() = default;

    //! No copying.	No assignment.
    SubspaceMapDelegate(const SubspaceMapDelegate& other) = delete;
    SubspaceMapDelegate& operator=(const SubspaceMapDelegate& other) = delete;

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent Owner for the editor.
	 *      @param [in] option Contains options for the editor.
	 *      @param [in] index Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, QModelIndex const& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor Pointer to the editor where the data is to be set.
	 *      @param [in] index Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor Pointer to the editor that contains the data to store.
	 *      @param [in] model Model that contains the data structure where data is to be saved to.
	 *      @param [in] index Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

protected:

    /*!
     *  Gets the description column.
     */
    virtual int descriptionColumn() const;

private:

    /*!
     *  Get the available initiator/master bus interface names.
     *
     *      @return List of initiator/master bus interface names.
     */
    QStringList getInitiatorInterfaceNames() const;

    /*!
     *  Get the list of available segments.
     *
     *      @param [in] index   Index of the selected subspace map.
     *
     *      @return List of available segments.
     */
    QStringList getSegmentNames(QModelIndex const& index) const;

    /*!
     *  Get the address space of the selected bus interface.
     *
     *      @param [in] busName     Name of the selected bus interface.
     *
     *      @return Address of the selected bus interface.
     */
    QSharedPointer<AddressSpace> getAddressSpace(QString const& busName) const;

    /*!
     *  Get the column for block name.
     *
     *      @return Index of the name column.
     */
    virtual int nameColumn() const override final;

    /*!
     *  Get the column for block base address.
     *
     *      @return Index of the base address column.
     */
    virtual int baseAddressColumn() const override final;

    /*!
     *  Get the column for block is present.
     *
     *      @return Index of the is present column.
     */
    virtual int isPresentColumn() const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing subspace maps.
    SubspaceMapInterface* subspaceInterface_;

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;

    //! List of available address spaces.
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces_;

    //! The IP-XACT standard revision to comply to.
    Document::Revision docRevision_;
};

#endif // SUBSPACEMAPDELEGATE_H
