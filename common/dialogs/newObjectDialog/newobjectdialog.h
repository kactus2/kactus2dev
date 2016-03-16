//-----------------------------------------------------------------------------
// File: newobjetdialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// NewObjectDialog is used to query the VLNV and path of a newly created design
//-----------------------------------------------------------------------------

#ifndef NEWOBJECTDIALOG_H
#define NEWOBJECTDIALOG_H

#include <common/widgets/LibrarySelectorWidget/LibrarySelectorWidget.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QDialog>
#include <QPushButton>

class LibraryInterface;
class VLNVEditor;
class KactusAttributeEditor;

//-----------------------------------------------------------------------------
//! NewObjectDialog is used to query the VLNV and path of a newly created design
//-----------------------------------------------------------------------------
class NewObjectDialog: public QDialog
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] libInterface    The library interface.
     *      @param [in] type            New object type.
     *      @param [in] showAttributes  Flag for visible attributes.
     *      @param [in] parent          Owner of this dialog.
     */
    NewObjectDialog(LibraryInterface* libInterface, VLNV::IPXactType type, bool showAttributes,
        QWidget *parent = 0);

	/*!
     *  Set the vlnv for the editor.
	 *
     *      @param [in] vlnv    Reference to the vlnv to set.
	 */
	void setVLNV(const VLNV& vlnv);

    /*!
     *  Sets the Kactus attributes for the dialog.
     *
     *      @param [in] prodHier    Product hierarchy.
     *      @param [in] firmness	firmness.
     */
    void setKactusAttributes(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness);

    /*!
     *  Get the user inputted VLNV.
     */
    VLNV getVLNV();

    /*!
     *  Returns the product hierarchy attribute.
     */
    KactusAttribute::ProductHierarchy getProductHierarchy() const;

    /*!
     *  Returns the firmness.
     */
    KactusAttribute::Firmness getFirmness() const;

    /*!
     *  Get the user inputted path.
     */
    QString getPath();

    /*!
     *  Shows a pre-filled dialog with the version field set empty.
     *
     *      @return True, if the user pressed OK. False if the user pressed Cancel.
     */
    static bool saveAsDialog(QWidget* parent, LibraryInterface* lh, VLNV const& oldVLNV,
                             KactusAttribute::ProductHierarchy& prodHier, KactusAttribute::Firmness& firmness,
                             VLNV& vlnv, QString& directory);

    /*!
     *  Shows a pre-filled dialog with the version field set empty and attributes hidden.
     *
     *      @return True, if the user pressed OK. False if the user pressed Cancel.
     */
    static bool saveAsDialog(QWidget* parent, LibraryInterface* lh, VLNV const& oldVLNV, VLNV& vlnv,
                             QString& directory, const QString& windowTitle = QString("Save As"));

private slots:
    /*!
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

    /*!
     *  [Description].
     */
    void onContentChanged();

    /*!
     *  Called when the user presses OK.
     */
    virtual void accept();

    /*!
     *  Called when the product hierarchy has changed.
     */
    void onProductHierarchyChanged();

private:
    //! The library interface.
    LibraryInterface* lh_;

    //! Attribute editor.
    KactusAttributeEditor* attributeEditor_;

    //! The VLNV editor.
    VLNVEditor* vlnvEditor_;

	//! \brief The editor to select the directory to save to. 
	LibrarySelectorWidget* directoryEditor_;

    //! OK button.
    QPushButton* okButton_;
};

#endif // NEWOBJECTDIALOG_H
