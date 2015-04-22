/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsdelegate.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSDELEGATE_H
#define MEMORYMAPSDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! The delegate to provide editor for adding/removing/editing the memory maps of a component.
//-----------------------------------------------------------------------------
class MemoryMapsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	//! \brief The role to display interface column with QStringList.
	enum Roles {
		USER_DISPLAY_ROLE = Qt::UserRole
	};

    /*!
	 *  The constructor.
	 *
	 *      @param [in] remapStateNames     A list of names of the component remap states.
	 *      @param [in] parent              Pointer to the owner of the delegate.
	 */
	MemoryMapsDelegate(QStringList remapStateNames, QObject *parent);
	
	//! \brief The destructor
	virtual ~MemoryMapsDelegate();

    /*!
	 *  Create a new editor for the given item.
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
     *
     *      @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const;

    /*!
	 *  Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   The index of the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

    /*!
	 *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   The inex of the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

    /*!
     *  Update the list of remap state names.
     *
     *      @param [in] newRemapStateNames  The new list of remap state names.
     */
    void updateRemapStateNames(QStringList newRemapStateNames);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private slots:

    /*!
	 *  Commit the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:
	
	//! \brief No copying
	MemoryMapsDelegate(const MemoryMapsDelegate& other);

	//! \brief No assignment
	MemoryMapsDelegate& operator=(const MemoryMapsDelegate& other);

    //! A list of the components remap state names.
    QStringList remapStateNames_;
};

#endif // MEMORYMAPSDELEGATE_H
