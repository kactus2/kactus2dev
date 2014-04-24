//-----------------------------------------------------------------------------
// File: RibbonGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.02.2013
//
// Description:
// Ribbon group class.
//-----------------------------------------------------------------------------

#ifndef RIBBONGROUP_H
#define RIBBONGROUP_H

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMenu>

class Ribbon;

//-----------------------------------------------------------------------------
//! Ribbon group class.
//-----------------------------------------------------------------------------
class RibbonGroup : public QWidget
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] icon    The group icon.
     *      @param [in] title   The group title.
     *      @param [in] parent  The parent ribbon.
     */
    RibbonGroup(QIcon const& icon, QString const& title, Ribbon* parent);

    /*!
     *  Destructor.
     */
    ~RibbonGroup();

    /*!
     *  Sets an icon for the group.
     *
     *      @param [in] icon The icon to set.
     */
    void setGroupIcon(QIcon const& icon);

    /*!
     *  Sets the group collapsed.
     *
     *      @param [in] collapsed If true, the group is set collapsed.
     */
    void setCollapsed(bool collapsed);

    /*!
     *  Sets the group's auto-collapse on/off.
     *
     *      @param [in] autoCollapse If true, the auto-collapse is set on; otherwise off.
     */
    void setAutoCollapse(bool autoCollapse);

    /*!
     *  Returns true if the group is collapsed.
     */
    bool isCollapsed() const;

    /*!
     *  Returns true if the ribbon can auto-collapse.
     */
    bool canAutoCollapse() const;

protected:
    //! Draws the group.
    virtual void paintEvent(QPaintEvent* event);

    virtual bool event(QEvent* e);

    
private:
    // Disable copying.
    RibbonGroup(RibbonGroup const& rhs);
    RibbonGroup& operator=(RibbonGroup const& rhs);

    /*!
     *  Creates a widget for the given action.
     *
     *      @param [in] action   The action for which to create a widget.
     *      @param [in] rowSpan  How many rows the widget spans in the grid layout.
     *      @param [in] colSpan  How many columns the widget spans in the grid layout.
     *
     *      @return The created widget.
     */
    QWidget* createWidget(QAction* action, int rowSpan, int colSpan);

    /*!
     *  Adds a widget for the given action to the ribbon group.
     *
     *      @param [in] action The action for which to add a widget.
     */
    void addWidgetForAction(QAction* action);

    /*!
     *  Removes the widget that corresponds to the given action.
     *
     *      @param [in] action The action whose widget to remove.
     */
    void removeWidgetForAction(QAction* action);
    
    /*!
     *  Recreates the whole action layout.
     */
    void recreateLayout();
    
    enum
    {
        TITLE_MARGIN = 20,
        TITLE_HEIGHT = 25,
        MIN_LARGE_BUTTON_WIDTH = 56,
        HMARGIN = 2
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The title text.
    QString title_;

    //! The parent ribbon.
    Ribbon* parent_;

    //! The button layout.
    QGridLayout* layout_;

    //! The action for drop-down menu.
    QAction* dropDownAction_;

    //! The drop-down button (visible when the group cannot be displayed in full size).
    QToolButton* dropDownButton_;

    //! The popup menu.
    QMenu* dropDownMenu_;

    //! If true, the group can auto-collapse.
    bool autoCollapse_;

    //! If true, the group is collapsed.
    bool collapsed_;

    //! The current row and column.
    int curRow_;
    int curColumn_;

    //! The width of the group in uncollapsed state.
    int maxWidth_;

    //! Widget map.
    QMap<QAction*, QWidget*> widgetMap_;
};

//-----------------------------------------------------------------------------

#endif // RIBBONGROUP_H
