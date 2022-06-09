//-----------------------------------------------------------------------------
// File: ScriptInputEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// Text editor for script write.
//-----------------------------------------------------------------------------

#ifndef SCRIPTINPUTEDITOR_H
#define SCRIPTINPUTEDITOR_H

#include <QPlainTextEdit>

#include "ScriptingSideArea.h"

//-----------------------------------------------------------------------------
//! Text editor for script write.
//-----------------------------------------------------------------------------
class ScriptInputEditor : public QPlainTextEdit
{
    Q_OBJECT
public:

    /*!
     * The constructor.
     *
     *     @param [in] parent  The parent widget.
     */
    explicit ScriptInputEditor(QWidget* parent = nullptr);

    //! The destructor.
    virtual ~ScriptInputEditor() = default;

    /*!
     * Get the required width of the side area.
     *
     *     @return The width of the side area in pixels.
     */
    int sideAreaWidth() const;

    /*!
     * Handler for side area paint event.
     *
     *     @param [in]  The paint event.
     */
    void sideAreaPaintEvent(QPaintEvent* event);

    /*!
     * Get the text on selected lines.
     *
     *     @return The text on the currently selected lines.
     */
     QString getSelectedLines() const;

     /*!
      * Set the style for automatic indentation.
      *
      *     @param [in] useTabs     Whether to use tabs or spaces.
      *     @param [in] width       Number of spaces to use.
      */
      void setIndentStyle(bool useTabs, unsigned int width);

protected:

    //! Event handler for key press handling.
    virtual void keyPressEvent(QKeyEvent *e) override final;

    //! Event handler for painting.
    virtual void paintEvent(QPaintEvent *e) override final;

    //! Event handler for widget resize.
    void resizeEvent(QResizeEvent *event) override;


private slots:

    /*!
     * Update the side area width when the number of lines (blocks) change.
     *
     *     @param [in] newBlockCount  The number of blocks in the text.
     */
     void updateSideAreaWidth(int newBlockCount);

     //! Add highlight to currently selected lines.
      void highlightSelectedLines() const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    /*!
     * Handler for side area update event.
     *
     *     @param [in] rect     The area being updated.
     *     @param [in] dy       The change of y coordinate in the update.
     */
    void updateSideArea(const QRect &rect, int dy);

    //! Side area for the widget for holding line numbers.
    ScriptingSideArea* editorSideArea_;

    //! Use tabs or spaces for indent.
    bool useTabs_ = false;

    //! How many spaces to use for indent.
    int indentWidth_ = 4;



};

#endif // SCRIPTINPUTEDITOR_H
