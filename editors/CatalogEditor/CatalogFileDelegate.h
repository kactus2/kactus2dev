//-----------------------------------------------------------------------------
// File: CatalogFileDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.02.2017
//
// Description:
// Delegate to provide editors to edit catalog files.
//-----------------------------------------------------------------------------

#ifndef CATALOGFILEDELEGATE_H
#define CATALOGFILEDELEGATE_H

#include <common/widgets/vlnvEditor/VLNVDataTree.h>

#include <QPainter>
#include <QSharedPointer>
#include <QStyledItemDelegate>


class LibraryInterface;
class VLNVContentMatcher;

//-----------------------------------------------------------------------------
// ! Delegate to provide editors to edit catalog files.
//-----------------------------------------------------------------------------
class CatalogFileDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] library     The library of available IP-XACT documents.
	 *      @param [in] parent      The parent of this delegate.
	 */
	CatalogFileDelegate(LibraryInterface* library, QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~CatalogFileDelegate();
       
    /*!
     *  Returns the size hint for the given model index.
     */
    virtual QSize sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const;

    /*!
     *  Paint the memory maps.
     *
     *      @param [in] painter     The used painter.
     *      @param [in] option      Available style options.
     *      @param [in] index       The current index.
     */
    virtual void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;
    
    /*!
     *  Creates an editor for the given index.
     *
     *      @param [in] parent      The parent widget for the editor.
     *      @param [in] option      Format options for the editor.
     *      @param [in] index       The index whose content to edit in the editor.
     *
     *      @return Editor for the index.
     */
    virtual QWidget* createEditor(QWidget* parent,  QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;
  
private:

	//! No copying.
	CatalogFileDelegate(const CatalogFileDelegate& other);
	CatalogFileDelegate& operator=(const CatalogFileDelegate& other);
      
    /*!
     *  Updates the suggested items in the editor for the given index.
     *
     *      @param [in] index   The index whose content to suggest.
     */
    void updateSuggestedItems(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library of available IP-XACT documents.
    LibraryInterface* library_;

    //! Matcher for editor suggestions.
    VLNVContentMatcher* matcher_;

    //! The VLNV data tree.
    VLNVDataTree* dataTree_;
};

#endif // CATALOGFILEDELEGATE_H
