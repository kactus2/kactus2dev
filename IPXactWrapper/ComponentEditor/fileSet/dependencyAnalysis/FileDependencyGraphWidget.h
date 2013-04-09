//-----------------------------------------------------------------------------
// File: FileDependencyGraphWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.03.2013
//
// Description:
// Widget which encapsulates the dependency graph view and a scroll bar
// for scrolling the dependencies.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYGRAPHWIDGET_H
#define FILEDEPENDENCYGRAPHWIDGET_H

#include "FileDependencyGraphView.h"

#include <QWidget>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
//! File dependency graph widget.
//-----------------------------------------------------------------------------
class FileDependencyGraphWidget : public QWidget
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    FileDependencyGraphWidget(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~FileDependencyGraphWidget();

    /*!
     * Returns the view.
     */
    FileDependencyGraphView& getView();

private slots:
    /*!
     *  Updates the progress bar when the graph column count has changed.
     */
    void onGraphColumnScrollMaximumChanged(int count);

    /*!
     *  Updates the progress bar to match the given column width.
     */
    void onDependencyColumnPositionChanged(int width);

    /*!
     *  Shows a warning message on the information display.
     *
     *      @param [in] message The message to show.
     */
    void showWarningMessage(QString const& message);

private:
    // Disable copying.
    FileDependencyGraphWidget(FileDependencyGraphWidget const& rhs);
    FileDependencyGraphWidget& operator=(FileDependencyGraphWidget const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graph view.
    FileDependencyGraphView view_;

    //! The scroll bar for dependencies.
    QScrollBar scrollBar_;

    //! The scroll bar layout.
    QHBoxLayout* barLayout_;

    //! The information label.
    QLabel infoLabel_;
};

#endif // FILEDEPENDENCYGRAPHWIDGET_H
