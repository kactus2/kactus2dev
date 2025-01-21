//-----------------------------------------------------------------------------
// File: FieldSliceDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 01.09.2023
//
// Description:
// Delegate that provides widgets for editing field slices in mode.
//-----------------------------------------------------------------------------

#ifndef FIELDSLICE_DELEGATE_H
#define FIELDSLICE_DELEGATE_H

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMap.h>

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <QSharedPointer>

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! Delegate that provides widgets for editing remap condition.
//-----------------------------------------------------------------------------
class FieldSliceDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] portNameList            The list of available port names.
     *    @param [in] completionModel         Model containing the completions used in expression editor.
     *    @param [in] expressions             The collection of objects for expression handling.
     *    @param [in] parent                  The parent of the object.
     */
    FieldSliceDelegate(QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps, 
		QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces,
        QAbstractItemModel* completionModel,
        QSharedPointer<ParameterFinder> parameterFinder,
		QObject* parent = nullptr);

	//! The destructor
    virtual ~FieldSliceDelegate() = default;

    //! No copying
    FieldSliceDelegate(const FieldSliceDelegate& other) = delete;

    //! No assignment
    FieldSliceDelegate& operator=(const FieldSliceDelegate& other) = delete;

	/*! Create a new editor for the given item
	 *
	 *    @param [in] parent   Owner for the editor.
	 *    @param [in] option   Contains options for the editor.
	 *    @param [in] index    Model index identifying the item.
	 *
	 *    @return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const final;

	/*! Set the data for the editor.
	 *
	 *    @param [in] editor  Pointer to the editor where the data is to be set.
	 *    @param [in] index   Model index identifying the item that's data is to be set.
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const final;

	/*! Save the data from the editor to the model.
	 *
	 *    @param [in] editor   Pointer to the editor that contains the data to store.
	 *    @param [in] model    Model that contains the data structure where data is to be saved to.
	 *    @param [in] index    Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const final;




	void updateEditorGeometry(QWidget* editor, QStyleOptionViewItem const& option, QModelIndex const& index) const override;


signals:

    //! Emitted when the values of the array change.
    void contentChanged();
protected:
    int descriptionColumn() const override;


	bool columnAcceptsExpression(int column) const override;

private slots:

	void commitAndCloseEditor();

	void onEditorCancel();

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------


    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps_;

	QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces_;

};

#endif // FIELDSLICE_DELEGATE_H
