//-----------------------------------------------------------------------------
// File: SaveHierarchyDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 22.05.2015
//
// Description:
// Delegate class for save hierarchy document selection.
//-----------------------------------------------------------------------------

#ifndef SAVEHIERARCHYDELEGATE_H
#define SAVEHIERARCHYDELEGATE_H

#include <QStyledItemDelegate>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Delegate class for save hierarchy document selection.
//-----------------------------------------------------------------------------
class SaveHierarchyDelegate : public QStyledItemDelegate
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] library     The document library to use.
	 *      @param [in] parent      The parent object.
	 */
	SaveHierarchyDelegate(LibraryInterface* library, QObject* parent);

	//! The destructor.
	virtual ~SaveHierarchyDelegate();

    /*! Create a new editor for the given item
	 *
	 *      @param [in] parent      Owner for the editor.
	 *      @param [in] option      Contains options for the editor.
	 *      @param [in] index       Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
	*/
    virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index) const;
  
    /*! Set the data for the editor.
	 *
	 *      @param [in] editor      The editor where the data is to be set.
	 *      @param [in] index       Model index identifying the item that's data is to be set.
	 *
	*/
    virtual void setEditorData(QWidget* editor,  QModelIndex const& index) const;

    /*! Save the data from the editor to the model.
	 *
	 *      @param [in] editor      The editor that contains the data to store.
	 *      @param [in] model       Model that contains the data structure where data is to be saved to.
	 *      @param [in] index       Model index identifying the item that's data is to be saved.
	 *
	*/
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

    /*!
     *  Finds the preferred size for an item.
     *
     *      @param [in] option      Style options for the item.
     *      @param [in] index       The model index whose size to find.
     *
     *      @return The preferred size for the item.
     */
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*!
     *  Draws an item.
     *
     *      @param [in] painter     The painter to use for drawing.
     *      @param [in] option      Style options for the item.
     *      @param [in] index       The item to draw.
     */
    virtual void paint(QPainter *painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

private:

	// Disable copying.
	SaveHierarchyDelegate(SaveHierarchyDelegate const& rhs);
	SaveHierarchyDelegate& operator=(SaveHierarchyDelegate const& rhs);
   
    //! The used document library.
    LibraryInterface* library_;
};

#endif // SAVEHIERARCHYDELEGATE_H
