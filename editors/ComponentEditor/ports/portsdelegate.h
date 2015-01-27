//-----------------------------------------------------------------------------
// File: portsdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.04.2011
//
// Description:
// The delegate that provides editors to edit ports of a component.
//-----------------------------------------------------------------------------

#ifndef PORTSDELEGATE_H
#define PORTSDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit ports of a component.
//-----------------------------------------------------------------------------
class PortsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:


	/*! The constructor
	 *
	 * @param [in] parent Pointer to the owner of this instance.
	 *
	*/
	PortsDelegate(QObject *parent = 0);
	
	//! The destructor
	virtual ~PortsDelegate();

	/*! Create a new editor for the given item
	 *
	 * @param [in] parent   Owner for the editor.
	 * @param [in] option   Contains options for the editor.
	 * @param [in] index    Model index idetifying the item.
	 *
	 * @return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 * @param [in] editor   Pointer to the editor where the data is to be set.
	 * @param [in] index    Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 * @param [in] editor   Pointer to the editor that contains the data to store.
	 * @param [in] model    Model that contains the data structure where data is to be saved to.
	 * @param [in] index    Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		QModelIndex const& index) const;

	virtual void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

protected:
    
    virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option, 
        QModelIndex const& index);

signals: 

    /*!
     *  Increase the amount of references to a parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to a parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void decreaseReferences(QString id);

private:
    // Disable copying.
    PortsDelegate(PortsDelegate const& rhs);
    PortsDelegate& operator=(PortsDelegate const& rhs);

    QWidget* createSelectorForDirection(QWidget* parent) const;
        
    QWidget* createSelectorWithVHDLTypes(QWidget* parent) const;
    
    QWidget* createSelectorWithVHDLStandardLibraries(QWidget* parent) const;

    //! Boolean for ad-hoc group modify.
    bool adhocGroupModify_;

    //! The new state for the group modify.
    Qt::CheckState adhocGroupState_;
};

#endif // PORTSDELEGATE_H
