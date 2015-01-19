/* 
 *
 *  Created on: 1.4.2011
 *      Author: Antti Kamppi
 * 		filename: portsdelegate.h
 */

#ifndef PORTSDELEGATE_H
#define PORTSDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
// Constants defining which column represents what kind of information.
//-----------------------------------------------------------------------------
enum PortEditorColumn
{
    PORT_COL_ROW = 0,           //!< Column for the row number.
    PORT_COL_NAME ,             //!< Column for the port name.
    PORT_COL_DIRECTION,         //!< Column for the port direction.
    PORT_COL_LEFT,              //!< Column for specifying the left bound of the port.
    PORT_COL_RIGHT,             //!< Column for specifying the right bound of the port.
    PORT_COL_WIDTH,             //!< Column for the port width.
    PORT_COL_TYPENAME,          //!< Column for the port typename.
    PORT_COL_TYPEDEF,           //!< Column for the port type definition.
    PORT_COL_DEFAULT,           //!< Column for setting the default value for the port.
    PORT_COL_DESC,              //!< Column for adding a description for the port.
    PORT_COL_ADHOC_VISIBILITY,  //!< Column for toggling ad-hoc visibility on/off.
    PORT_COL_COUNT
};

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit ports of a component.
//-----------------------------------------------------------------------------
class PortsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this instance.
	 *
	*/
	PortsDelegate(QObject *parent = 0);
	
	//! \brief The destructor
	virtual ~PortsDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index idetifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index) const;

	/*! \brief Set the data for the editor.
	 *
	 * \param editor Pointer to the editor where the data is to be set.
	 * \param index Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*! \brief Save the data from the editor to the model.
	 *
	 * \param editor Pointer to the editor that contains the data to store.
	 * \param model Model that contains the data structure where data is to be saved to.
	 * \param index Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		const QModelIndex& index) const;

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

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

private slots:

	/*! \brief Commit the data from the sending editor and close the editor.
	*
	*/
	void commitAndCloseEditor();

private:
    // Disable copying.
    PortsDelegate(PortsDelegate const& rhs);
    PortsDelegate& operator=(PortsDelegate const& rhs);

    //! Boolean for ad-hoc group modify.
    bool adhocGroupModify_;

    //! The new state for the group modify.
    Qt::CheckState adhocGroupState_;
};

#endif // PORTSDELEGATE_H
