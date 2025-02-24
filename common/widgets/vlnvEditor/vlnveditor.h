//-----------------------------------------------------------------------------
// File: vlnveditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 7.2.2011
//
// Description:
// VLNV editor group box.
//-----------------------------------------------------------------------------

#ifndef VLNVEDITOR_H
#define VLNVEDITOR_H

#include "VLNVDataTree.h"
#include "VLNVContentMatcher.h"

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QGroupBox>
#include <QLineEdit>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QLabel>

class LibraryInterface;
class LibraryItem;
class AssistedLineEdit;

//-----------------------------------------------------------------------------
//! VLNVEditor class.
//-----------------------------------------------------------------------------
class VLNVEditor : public QGroupBox
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] type        The VLNV type which to edit.
     *    @param [in] libHandler  The library handler.
     *    @param [in] parentWnd   The parent window (having a title bar).
     *    @param [in] parent      The parent widget.
     */
    VLNVEditor(VLNV::IPXactType type, LibraryInterface* libHandler, QWidget* parentWnd = 0, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~VLNVEditor();

    /*!
     *  Adds a new VLNV object type to show in content assist.
     */
    void addContentType(VLNV::IPXactType type);

    /*!
     *  Sets the firmness filter on/off.
     *
     *    @param [in] on        If true, the filter is turned on; otherwise it is turned off.
     *    @param [in] firmness  The allowed firmness.
     */
    void setFirmnessFilter(bool on, KactusAttribute::Firmness firmness = KactusAttribute::TEMPLATE);

    /*!
     *  Sets the product hierarchy filter on/off.
     *
     *    @param [in] on           If true, the filter is turned on; otherwise it is turned off.
     *    @param [in] productHier  The allowed product hierarchy.
     */
    void setHierarchyFilter(bool on, KactusAttribute::ProductHierarchy productHier = KactusAttribute::IP);

    /*!
     *  Sets the implementation filter on/off.
     *
     *    @param [in] on              If true, the filter is turned on; otherwise it is turned off.
     *    @param [in] implementation  The allowed implementation.
     */
    void setImplementationFilter(bool on, KactusAttribute::Implementation implementation = KactusAttribute::HW);

    void setRevisionFilter(bool on, Document::Revision revision = Document::Revision::Std22);

    /*!
     *  Adds a new name extension that will be shown in the dialog.
     *
     *    @param [in] extension The extension to add.
     */
    void addNameExtension(QString const& extension);

    /*!
     *  Generates a VLNV that matches the contents of the editor.
     */
    VLNV getVLNV() const;

    /*!
     *  Returns true if the contents of the editor are valid for a VLNV.
     */
    bool isValid() const;

	/*!
	 * Checks if the editor contains text in any field.
	 */
	bool isEmpty() const;

signals:

    /*!
     *  Emitted when the contents of the editor have changed.
     */
    void contentChanged();

    /*!
     *  Emitted when the user edits the VLNV in the editor.
     */
    void vlnvEdited();

	/*!
     *  Emitted when a vlnv for abstraction definition is dropped to this editor.
	 *
	 *    @param [in] busDefVLNV The vlnv of the matching bus definition.
	 */
	void setBusDef(const VLNV& busDefVLNV);

	/*!
     *  Emitted when a vlnv for bus definition is dropped to this editor. If there are several matching
     *  abstraction definitions then this is not emitted.
	 * 
     *    @param [in] absDefVLNV  Identifies the vlnv of matching abstraction definition.
	 */
	void setAbsDef(const VLNV& absDefVLNV);

public slots:
	
    /*!
     *  Set the widget to be mandatory or not. The default setting is mandatory on.
	 *
	 *    @param [in] mandatory   If true then all 4 vlnv fields are displayed as mandatory fields.
	 */
	void setMandatory(bool mandatory);

	/*! 
	 *  Set the contents of the editor to match the given VLNV.
	 *
	 *		@param [in] vlnv The VLNV. 
	 */
	void setVLNV(VLNV const& vlnv);

    /*!
     *  Updates the data tree.
     */
    void updateFiltering();

    /*!
     *  Updates all matcher items based on the contents of the VLNV editor fields.
     */
    void updateMatcherItems();

    /*!
     *  Called when the vendor field has changed.
     */
    void updateLibraryMatcherItem();

    /*!
     *  Called when the library field has changed.
     */
    void updateNameMatcherItem();

    /*!
     *  Called when the name field has changed.
     */
    void updateVersionMatcherItem();

	/*!
     *  Sets the vendor-field to contain the given string.
     */
	void setVendor(QString const& vendor);

	/*!
     *  Sets the library-field to contain the given string.
     */
	void setLibrary(QString const& library);

	/*!
     *  Sets the name-field to contain the given string.
     */
	void setName(QString const& name);

	/*!
     *  Sets the version-field to contain the given string.
     */
	void setVersion(QString const& version);

protected:
	
    /*!
     *  Handler for drop events on drag & drop
	 *
	 *    @param [in] event   Pointer to the drop event.
	 */
	virtual void dropEvent(QDropEvent* event);

	/*!
     *  Handler for drag enter events in drag & drop.
	 *
	 *    @param [in] event   Pointer to the event.
	 */
	virtual void dragEnterEvent(QDragEnterEvent* event);
    
    /*!
     *  Handler for the show event.
     *
     *    @param [in] event   Pointer to the show event.
     */
    virtual void showEvent(QShowEvent* event);

private slots:

    /*!
     *  Refreshes the filtering and matcher items.
     */
    void refresh();

private:
    // Disable copying.
    VLNVEditor(VLNVEditor const& rhs);
    VLNVEditor& operator=(VLNVEditor const& rhs);

    /*!
     *  Initializes the Qt connections between signals and slots.
     */
    void initConnections();

    /*!
     *  Initializes the widgets of the vlnv editor.
     *
     *    @param [in] parentWnd   The parent window.
     */
    void initWidgets(QWidget* parentWnd);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The VLNV type which to edit.
    VLNV::IPXactType type_;

    //! The VLNV types which to show in content assist.
    QList<VLNV::IPXactType> contentTypes_;

    //! True if the filtering is "dirty" and needs to be updated.
    bool dirty_;

    //! The VLNV data tree.
    VLNVDataTree dataTree_;

    //! Line edit for the vendor element.
    AssistedLineEdit* vendorEdit_;

    //! Content matcher for the vendor field.
    VLNVContentMatcher vendorMatcher_;

    //! Line edit for the library element.
    AssistedLineEdit* libraryEdit_;

    //! Content matcher for the library field.
    VLNVContentMatcher libraryMatcher_;

    //! Line edit for the name element.
    AssistedLineEdit* nameEdit_;

    //! Name extension label.
    QLabel nameExtensionLabel_;

    //! Content matcher for the name field.
    VLNVContentMatcher nameMatcher_;

    //! Line edit for the version element.
    AssistedLineEdit* versionEdit_;

    //! Content matcher for the version field.
    VLNVContentMatcher versionMatcher_;

	//! Pointer to the instance that manages the library
	LibraryInterface* handler_;

    //! Implementation filter data.
    bool implementationFilterEnabled_ = false;

    //! Implementation filter.
    KactusAttribute::Implementation implementationFilter_;

    //! Revision filter.
    bool revisionFilterEnabled_ = false;
    Document::Revision revisionFilter_;
};

//-----------------------------------------------------------------------------

#endif // VLNVEDITOR_H