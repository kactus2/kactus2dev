//-----------------------------------------------------------------------------
// File: vlnveditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.2.2011
//
// Description:
// VLNV editor group box.
//-----------------------------------------------------------------------------

#ifndef VLNVEDITOR_H
#define VLNVEDITOR_H

#include "VLNVDataTree.h"

#include <LibraryManager/vlnv.h>

#include <QGroupBox>
#include <QLineEdit>
#include <QDropEvent>
#include <QDragEnterEvent>

class LibraryInterface;
class LibraryItem;
class AssistedLineEdit;
class VLNVContentMatcher;

//-----------------------------------------------------------------------------
//! VLNVEditor class.
//-----------------------------------------------------------------------------
class VLNVEditor : public QGroupBox
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] vlnvType    The VLNV type which to edit.
     *      @param [in] parentWnd   The parent window (having a title bar).
     *      @param [in] libHandler  The library handler.
     *      @param [in] parent      The parent widget.
     *      @param [in] compact     If true, the VLNV layout is made compact (label and edit on the same row).
     *                              If false, the labels and edits are on different rows.
     */
    VLNVEditor(VLNV::IPXactType type, LibraryInterface* libHandler, QWidget* parentWnd = 0,
               QWidget* parent = 0, bool compact = true);

    /*!
     *  Destructor.
     */
    virtual ~VLNVEditor();

    /*!
     *  Sets the firmness filter on/off.
     *
     *      @param [in] on        If true, the filter is turned on; otherwise it is turned off.
     *      @param [in] firmness  The allowed firmness.
     */
    void setFirmnessFilter(bool on, KactusAttribute::Firmness firmness = KactusAttribute::KTS_TEMPLATE);

    /*!
     *  Sets the product hierarchy filter on/off.
     *
     *      @param [in] on           If true, the filter is turned on; otherwise it is turned off.
     *      @param [in] productHier  The allowed product hierarchy.
     */
    void setHierarchyFilter(bool on, KactusAttribute::ProductHierarchy productHier = KactusAttribute::KTS_IP);

    /*!
     *  Sets the implementation filter on/off.
     *
     *      @param [in] on              If true, the filter is turned on; otherwise it is turned off.
     *      @param [in] implementation  The allowed implementation.
     */
    void setImplementationFilter(bool on, KactusAttribute::Implementation implementation = KactusAttribute::KTS_HW);

    /*!
     *  Generates a VLNV that matches the contents of the editor.
     *
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

    //! Emitted when the contents of the editor have changed.
    void contentChanged();

    //! Emitted when the user edits the VLNV in the editor.
    void vlnvEdited();

	/*! \brief Emitted when a vlnv for abstraction definition is dropped to this editor.
	 *
	 * \param busDefVLNV The vlnv of the matching bus definition.
	 *
	*/
	void setBusDef(const VLNV& busDefVLNV);

	/*! \brief Emitted when a vlnv for bus definition is dropped to this editor.
	 * 
	 * If there are several matching abstraction definitions then this is not emitted.
	 * 
	 * \param absDefVLNV Identifies the vlnv of matching abstraction definition.
	 *
	*/
	void setAbsDef(const VLNV& absDefVLNV);

public slots:

	/*! \brief Set the widget to be mandatory or not.
	 * 
	 * The default setting is mandatory on.
	 * 
	 * \param mandatory If true then all 4 vlnv fields are displayed as mandatory fields.
	 *
	*/
	void setMandatory(bool mandatory);

	/*!
     *  Sets the contents of the editor to match the given VLNV.
     *
     *      @param [in] vlnv The VLNV.
     */
    void setVLNV(VLNV const* vlnv);

	/*! 
	 *  Set the contents of the editor to match the given VLNV.
	 *
	 *		@param [in] vlnv The VLNV. 
	 */
	void setVLNV(const VLNV& vlnv);

    //! Updates the data tree.
    void updateFiltering();

    //! Updates all matcher items based on the contents of the VLNV editor fields.
    void updateMatcherItems();

    //! Called when the vendor field has changed.
    void updateLibraryMatcherItem();

    //! Called when the library field has changed.
    void updateNameMatcherItem();

    //! Called when the name field has changed.
    void updateVersionMatcherItem();

	//! \brief Sets the vendor-field to contain the given string.
	void setVendor(const QString& vendor);

	//! \brief Sets the library-field to contain the given string.
	void setLibrary(const QString& library);

	//! \brief Sets the name-field to contain the given string.
	void setName(const QString& name);

	//! \brief Sets the version-field to contain the given string.
	void setVersion(const QString& version);

protected:

	/*! \brief Handler for drop events on drag & drop
	 *
	 * \param event Pointer to the drop event.
	 *
	*/
	virtual void dropEvent(QDropEvent* event);

	/*! \brief Handler for drag enter events in drag & drop.
	 *
	 * \param event Pointer to the event.
	 *
	*/
	virtual void dragEnterEvent(QDragEnterEvent* event);

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
     *      @param [in] parentWnd The parent window.
     *      @param [in] compact   If true, the layout is made compact.
     */
    void initWidgets(QWidget* parentWnd, bool compact);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The VLNV type which to edit.
    VLNV::IPXactType type_;

    //! The VLNV data tree.
    VLNVDataTree dataTree_;

    //! Line edit for the vendor element.
    AssistedLineEdit* vendorEdit_;

    //! Content matcher for the vendor field.
    QSharedPointer<VLNVContentMatcher> vendorMatcher_;

    //! Line edit for the library element.
    AssistedLineEdit* libraryEdit_;

    //! Content matcher for the library field.
    QSharedPointer<VLNVContentMatcher> libraryMatcher_;

    //! Line edit for the name element.
    AssistedLineEdit* nameEdit_;

    //! Content matcher for the name field.
    QSharedPointer<VLNVContentMatcher> nameMatcher_;

    //! Line edit for the version element.
    AssistedLineEdit* versionEdit_;

    //! Content matcher for the version field.
    QSharedPointer<VLNVContentMatcher> versionMatcher_;

	//! \brief Pointer to the instance that manages the library
	LibraryInterface* handler_;
};

//-----------------------------------------------------------------------------

#endif // VLNVEDITOR_H