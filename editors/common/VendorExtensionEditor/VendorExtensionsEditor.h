//-----------------------------------------------------------------------------
// File: VendorExtensionsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.12.2018
//
// Description:
// Editor to display/edit vendor extensions.
//-----------------------------------------------------------------------------

#ifndef VENDOREXTENSIONSEDITOR_H
#define VENDOREXTENSIONSEDITOR_H

#include <QList>
#include <QSharedPointer>
#include <QWidget>
#include <QLabel>

class EditableTreeView;
class VendorExtensionsModel;
class VendorExtension;
class TabDocument;
class Extendable;

//-----------------------------------------------------------------------------
//! Editor to display/edit vendor extensions.
//-----------------------------------------------------------------------------
class VendorExtensionsEditor : public QWidget
{
    Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent      The owner of this widget.     
	 */
	VendorExtensionsEditor(QWidget *parent);
	
	//! The destructor.
	virtual ~VendorExtensionsEditor() = default;

    //! No copying.
    VendorExtensionsEditor(const VendorExtensionsEditor& other) = delete;

    //! No assignment.
    VendorExtensionsEditor& operator=(const VendorExtensionsEditor& other) = delete;

    /*!
     * Locks/Unlocks the editor.
     *
     *     @param [in] locked  The lock state to set.
     */
    void setLocked(bool locked);
    
    /*!
     * Set the context for the editor.
     *
     *     @param [in] doc  The document widget whose vendor extensions are being edited.     
     */
     void setContext(TabDocument* doc);

signals:

    //! Informs of a content change in the editor.
    void contentChanged();

public slots:

	//! Clear the editor so no extensions are shown
    void clear();

    /*!
     *  Change the vendor extensions of the vendor extensions editor.
     *
     *      @param [in] containingID    ID for the vendor extensions editor.
     *      @param [in] extensionItem   The item containing the selected vendor extensions.
     */
    void changeVendorExtensions(QString const& containingID, QSharedPointer<Extendable> extensionItem);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The view for the vendor extensions table.
    EditableTreeView* extensionsView_;

    //! The model for the vendor extensions.
    VendorExtensionsModel* extensionsModel_;

    //! Holds the ID for the editor.
    QLabel* summaryLabel_;
};

#endif // VENDOREXTENSIONSEDITOR_H
