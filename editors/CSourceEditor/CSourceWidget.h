//-----------------------------------------------------------------------------
// File: CSourceWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 9.5.2011
//
// Description:
// C source widget.
//-----------------------------------------------------------------------------

#ifndef MCAPISOURCEWIDGET_H
#define MCAPISOURCEWIDGET_H

#include "CSourceHighlighter.h"

#include <common/widgets/tabDocument/TabDocument.h>

#include <QSharedPointer>
#include <QList>

class CSourceTextEdit;
class TextEditProvider;
class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! CSourceWidget class.
//-----------------------------------------------------------------------------
class CSourceWidget : public TabDocument
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] sourceFile      The name of the source file in the file system.
     *      @param [in] ownerComponent  The component who "owns" the source file that will be edited.
     *      @param [in] libInterface    The library interface.
     *      @param [in] mainWnd         The main window.
     *      @param [in] parent          The parent widget.
     *
     *      @remarks The file is assumed to exist in the file system.
     */
    CSourceWidget(QString const& sourceFile,
                  QSharedPointer<Component> ownerComponent,
                  LibraryInterface* libInterface,
                  QWidget* mainWnd, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~CSourceWidget();

	/*! \brief Get the VLNV that can be used to identify the document.
	 *
	 * \return The VLNV that identifies the document.
	*/
	virtual VLNV getIdentifyingVLNV() const;

    /*!
     *  Applies the current settings into use.
     */
    void applySettings(QSettings& settings) override;

    /*!
     *  Returns the name of the source file.
     */
    QString const& getSourceFile() const;

    /*!
     *  Returns the edit provider.
     */
    virtual QSharedPointer<IEditProvider> getEditProvider() const;
        
    virtual void refresh();

public slots:
    //! Saves the source file.
    virtual bool save();

    //! Prints the source.
    void print();

    //! Called when the contents of the document have changed.
    void onContentChanged();

private:
    // Disable copying.
    CSourceWidget(CSourceWidget const& rhs);    
    CSourceWidget& operator=(CSourceWidget const& rhs);

    //! Reads the source file content into the editor.
    void readFileContentFromDisk();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The MCAPI text edit.
    CSourceTextEdit* textEdit_;

    //! The source filename.
    QString sourceFile_;

    //! Edit provider for the text edit.
    QSharedPointer<TextEditProvider> editProvider_;
};

//-----------------------------------------------------------------------------

#endif // MCAPISOURCEWIDGET_H
