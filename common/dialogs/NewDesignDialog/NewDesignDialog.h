//-----------------------------------------------------------------------------
// File: NewDesignDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.8.2012
//
// Description:
// Dialog used in creation of new designs to existing components.
//-----------------------------------------------------------------------------

#ifndef NEWDESIGNDIALOG_H
#define NEWDESIGNDIALOG_H

#include <LibraryManager/vlnv.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/KactusAttribute.h>

#include <QDialog>
#include <QPushButton>

class LibraryPathSelector;
class LibraryInterface;
class VLNVEditor;
class Component;
class LineEditEx;

//-----------------------------------------------------------------------------
//! New design dialog.
//-----------------------------------------------------------------------------
class NewDesignDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface  The library interface.
     *      @param [in] component     The component for which the design will be created.
     *      @param [in] designType    The type of design to create.
     *      @param [in] parent        The parent widget.
     */
    NewDesignDialog(LibraryInterface* libInterface, QSharedPointer<Component> component,
                    KactusAttribute::Implementation designType, QWidget *parent = 0);

    /*!
     *  Sets the pre-filled view name.
     *
     *      @param [in] viewName The view name to set.
     */
    void setViewName(QString const& viewName);

	/*!
     *  Sets the pre-filled VLNV.
     *
     *      @param [in] vlnv The VLNV to set.
     */
	void setVLNV(const VLNV& vlnv);

    /*!
     *  Returns the view name.
     */
    QString getViewName() const;

    /*!
     *  Returns the design VLNV.
     */
    VLNV getDesignVLNV() const;

    /*!
     *  Returns the design configuration VLNV.
     */
    VLNV getDesignConfVLNV() const;

    /*! 
     *  Returns the path where to save the design.
     */
    QString getPath() const;

private slots:
    /*!
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

    /*!
     *  Updates the VLNV name based on the current view name.
     */
    void updateVlnvName();

    void onContentChanged();

    /*!
     *  Called when the user presses OK.
     */
    virtual void accept();
private:
    //! The library interface.
    LibraryInterface* lh_;

    //! The component for which the design will be created.
    QSharedPointer<Component> component_;

    //! The used view names.
    QStringList usedViewNames_;

    //! Label for the view name.
    QLabel* viewNameLabel_;

    //! Edit box for the view name.
    LineEditEx* viewNameEdit_;

    //! The VLNV editor.
    VLNVEditor* vlnvEditor_;

	//! \brief The editor to select the directory to save to. 
	LibraryPathSelector* directoryEdit_;

    //! OK button.
    QPushButton* okButton_;

    //! Extension for design object.
    QString designExt_;

    //! Extension for designcfg object.
    QString designConfExt_;
};

//-----------------------------------------------------------------------------

#endif // NEWDESIGNDIALOG_H
