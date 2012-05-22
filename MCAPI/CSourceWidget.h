//-----------------------------------------------------------------------------
// File: CSourceWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.5.2011
//
// Description:
// C source widget.
//-----------------------------------------------------------------------------

#ifndef MCAPISOURCEWIDGET_H
#define MCAPISOURCEWIDGET_H

#include "MCAPIHighlighter.h"

#include <common/widgets/tabDocument/TabDocument.h>

#include <QSharedPointer>
#include <QList>

class CSourceTextEdit;
class MCAPIContentMatcher;
class TextEditProvider;

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
     *      @param [in] sourceFile    The name of the source file in the file system.
     *      @param [in] mcapiMatcher  The content matcher.
     *      @param [in] mainWnd       The main window.
     *      @param [in] parent        The parent widget.
     *
     *      @remarks The file is assumed to exist in the file system.
     */
    CSourceWidget(QString const& sourceFile,
                      QSharedPointer<MCAPIContentMatcher> mcapiMatcher,
                      QWidget* mainWnd, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~CSourceWidget();

    /*!
     *  Applies the current settings into use.
     */
    void applySettings(QSettings const& settings);

    /*!
     *  Returns the name of the source file.
     */
    QString const& getSourceFile() const;

    /*!
     *  Returns the edit provider.
     */
    virtual IEditProvider* getEditProvider();

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
