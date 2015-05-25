//-----------------------------------------------------------------------------
// File: ViewConfigurerDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2015
//
// Description:
// Delegate for view configurer.
//-----------------------------------------------------------------------------

#ifndef VIEWCONFIGURERDELEGATE_H
#define VIEWCONFIGURERDELEGATE_H

#include <QStyledItemDelegate>

class LibraryHandler;
//-----------------------------------------------------------------------------
//! Delegate for view configurer.
//-----------------------------------------------------------------------------
class ViewConfigurerDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] libraryHandler  The library manager.
     *      @param [in] parent          The parent of this delegate.
     */
    ViewConfigurerDelegate(LibraryHandler* libraryHandler, QObject* parent);

    /*!
     *  The destructor.
     */
    ~ViewConfigurerDelegate();

    /*!
	 *  Create a new editor for the given item.
	 *
	 *      @param [in] parent  Owner of the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
     *
     *      @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

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
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item thats data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

    /*!
     *  Paints the delegate.
     *
     *      @param [in] painter     The painter used.
     *      @param [in] option      Options for the painter.
     *      @param [in] index       Model index identifying the item thats data is to be saved.
     */
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    // Disable copying and assignment.
    ViewConfigurerDelegate(ViewConfigurerDelegate const& rhs);
    ViewConfigurerDelegate& operator=(ViewConfigurerDelegate const& rhs);

    /*!
     *  Create the view selector the selected item.
     *
     *      @param [in] currentIndex    Model Index of the item.
     *      @param [in] parent          The owner of the item.
     *
     *      @return View selector for the selected item.
     */
    QWidget* createViewSelector(QModelIndex const& currentIndex, QWidget* parent) const;

    //! The library manager.
    LibraryHandler* libraryHandler_;
};

#endif // VIEWCONFIGURERDELEGATE_H
