//-----------------------------------------------------------------------------
// File: ConnectedPortsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.02.2019
//
// Description:
// Delegate that provides widgets for editing connected port pairings.
//-----------------------------------------------------------------------------

#ifndef CONNETEDPORTSDELEGATE_H
#define CONNETEDPORTSDELEGATE_H

#include <IPXACTmodels/common/DirectionTypes.h>

class Component;

#include <QStyledItemDelegate>
#include <QComboBox>

//-----------------------------------------------------------------------------
//! Delegate that provides widgets for editing connected port pairings.
//-----------------------------------------------------------------------------
class ConnectedPortsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] firstComponent      The first port containing component.
     *      @param [in] secondComponent     The second port containing component.
	 *      @param [in] parent              The parent of the object.
	 */
	ConnectedPortsDelegate(QSharedPointer<Component> firstComponent, QSharedPointer<Component> secondComponent,
        QObject* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~ConnectedPortsDelegate();

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent   Owner for the editor.
	 *      @param [in] option   Contains options for the editor.
	 *      @param [in] index    Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
     */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *     @param [in] editor   Pointer to the editor that contains the data to store.
	 *     @param [in] model    Model that contains the data structure where data is to be saved to.
	 *     @param [in] index    Model index identifying the item that's data is to be saved.
     */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

signals:

    /*!
     *  Emitted when the values of the array change.
     */
    void contentChanged();

private:

	//! No copying. No assignment.
	ConnectedPortsDelegate(const ConnectedPortsDelegate& other);
    ConnectedPortsDelegate& operator=(const ConnectedPortsDelegate& other);

    /*!
     *  Create the selection widget for ports.
     *
     *      @param [in] component   Component containing the selected ports.
     *      @param [in] parent      Parent item of the editor.
     *
     *      @return Combobox for component ports.
     */
    QComboBox* createPortSelector(QSharedPointer<Component> component, QWidget* parent) const;

    /*!
     *  Get the path of the icon for the selected port.
     *
     *      @param [in] portDirection    Direction of the selected port.
     *
     *      @return Path to the icon of the selected port.
     */
    QString getIconPath(DirectionTypes::Direction portDirection) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The first component.
    QSharedPointer<Component> firstComponent_;

    //! The second component.
    QSharedPointer<Component> secondComponent_;
};

#endif // CONNETEDPORTSDELEGATE_H
