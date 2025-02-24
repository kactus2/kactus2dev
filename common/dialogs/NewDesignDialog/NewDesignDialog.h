//-----------------------------------------------------------------------------
// File: NewDesignDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 2.8.2012
//
// Description:
// Dialog used in creation of new designs to existing components.
//-----------------------------------------------------------------------------

#ifndef NEWDESIGNDIALOG_H
#define NEWDESIGNDIALOG_H

#include <IPXACTmodels/common/VLNV.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/assistedLineEdit/BasicLineContentMatcher.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QDialog>
#include <QPushButton>

class LibrarySelectorWidget;
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
     *    @param [in] libInterface  The library interface.
     *    @param [in] component     The component for which the design will be created.
     *    @param [in] designType    The type of design to create.
     *    @param [in] parent        The parent widget.
     */
    NewDesignDialog(LibraryInterface* libInterface, QSharedPointer<Component> component,
                    KactusAttribute::Implementation designType, QWidget *parent = 0);
    

    /*!
     *  Sets the pre-filled view name.
     *
     *    @param [in] viewName The view name to set.
     */
    void setViewName(QString const& viewName);

    /*!
     *  Sets the view name suggestions.
     */
    void setViewNameSuggestions(QStringList const& suggestions);

	/*!
     *  Sets the pre-filled VLNV.
     *
     *    @param [in] vlnv The VLNV to set.
     */
	void setVLNV(const VLNV& vlnv);

    /*!
     *  Returns the qualifier name.
     */
    QString getQualifierName() const;

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
     *  Updates the View name based on the current qualifier.
     */
    void updateViewName();

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

    //! No copying
    NewDesignDialog(NewDesignDialog const& other);
    NewDesignDialog& operator=(NewDesignDialog const& other);

    //! Sets the dialog layout.
    void setupLayout();
    
    //! The library interface.
    LibraryInterface* library_;

    //! The component for which the design will be created.
    QSharedPointer<Component> component_;

    //! The used view names.
    QStringList usedViewNames_;

    //! Informational label at the top of the dialog.
    QLabel* introLabel_; 

    //! Label for the view name.
    QLabel* qualifierLabel_;

    //! Edit box for the view name.
    LineEditEx* qualifierEdit_;

    //! Label for the view name.
    QLabel* viewNameLabel_;

    //! View name content matcher.
    BasicLineContentMatcher viewNameMatcher_;

    //! Edit box for the view name.
    LineEditEx* viewNameEdit_;

    //! Icon for different design types.
    QLabel* designIcon_; 

    //! The VLNV editor.
    VLNVEditor* vlnvEditor_;

	//! \brief The editor to select the directory to save to. 
	LibrarySelectorWidget* directoryEditor_;

    //! OK button.
    QPushButton* okButton_;

    //! Extension for design object.
    QString designExt_;

    //! Extension for designcfg object.
    QString designConfExt_;
};

//-----------------------------------------------------------------------------

#endif // NEWDESIGNDIALOG_H
