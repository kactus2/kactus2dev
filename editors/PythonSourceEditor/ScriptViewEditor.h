//-----------------------------------------------------------------------------
// File: ScriptViewEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// Text editor for script write and run.
//-----------------------------------------------------------------------------

#ifndef SCRIPTVIEWEDITOR_H
#define SCRIPTVIEWEDITOR_H

#include <QPlainTextEdit>

//-----------------------------------------------------------------------------
//! Text editor for script write and run.
//-----------------------------------------------------------------------------
class ScriptViewEditor : public QPlainTextEdit
{
    Q_OBJECT
public:

    /*!
     * The constructor.
     *
     *     @param [in] parent  The parent widget.
     */
    explicit ScriptViewEditor(QWidget* parent = nullptr);

    //! The destructor.
    virtual ~ScriptViewEditor() = default;

public slots:

    /*! Print the given text in the editor.
     *
     *      @param [in] input  The text to print.
     */
    void printInput(QString const& input);

    /*! Print the given text in the editor.
     *
     *      @param [in] input  The text to print.
     */
    void print(QString const& input);

    /*! Print the given error text in the editor.
     *
     *      @param [in] input  The error text to print.
     */
    void printError(QString const& input);

private:

    /*!
     * Print the given input with given color.
     *
     *     @param [in] input        The input text to print.
     *     @param [in] textColor    The color of the text to use.
     */
     void printWithColor(QString const& input, QColor const& textColor);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The current prompt text.
    QString promptText_ = QString();

};

#endif // SCRIPTVIEWEDITOR_H
