//-----------------------------------------------------------------------------
// File: Ribbon.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.02.2013
//
// Description:
// Ribbon toolbar.
//-----------------------------------------------------------------------------

#ifndef RIBBONGROUPCONTAINER_H
#define RIBBONGROUPCONTAINER_H

#include <QWidget>
#include <QColor>
#include <QHBoxLayout>

class RibbonGroup;

//-----------------------------------------------------------------------------
//! Ribbon theme description.
//-----------------------------------------------------------------------------
struct RibbonTheme
{
    QColor gradientTop;
    QColor gradientBottom;
    QColor groupTitleGradientTop;
    QColor groupTitleGradientBottom;
    QColor groupTitleText;

    /*!
     *  Default constructor.
     */
    RibbonTheme() 
        : gradientTop(184, 210, 236),
          gradientBottom(130, 181, 232),
          groupTitleGradientTop(33, 135, 237),
          groupTitleGradientBottom(17, 127, 237),
          groupTitleText(Qt::white)
    {
    }
};

//-----------------------------------------------------------------------------
//! Ribbon toolbar.
//-----------------------------------------------------------------------------
class Ribbon : public QWidget
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    Ribbon(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~Ribbon();

    /*!
     *  Adds a group to the ribbon.
     *
     *      @param [in] title The title for the group.
     */
    RibbonGroup* addGroup(QString const& title);

    /*!
     *  Adds a group to the ribbon.
     *
     *      @param [in] icon  The icon for the group.
     *      @param [in] title The title for the group.
     */
    RibbonGroup* addGroup(QIcon const& icon, QString const& title);

    /*!
     *  Sets the theme for the ribbon.
     *
     *      @param [in] theme The theme to set.
     */
    void setTheme(RibbonTheme const& theme);

    /*!
     *  Returns the current theme for the ribbon.
     */
    RibbonTheme const& getTheme() const;

    /*!
     *  Updates the size of the container.
     */
    void updateSize();

    /*!
     *  Updates the collapsed state for optimal group layout.
     */
    void updateCollapse();

protected:
    //! Draws the ribbon background.
    virtual void paintEvent(QPaintEvent* event);

    virtual void resizeEvent(QResizeEvent* e);

private:
    // Disable copying.
    Ribbon(Ribbon const& rhs);
    Ribbon& operator=(Ribbon const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent ribbon.
    QWidget* parent_;

    //! The currently set ribbon theme.
    RibbonTheme theme_;

    //! The layout for the ribbon groups.
    QHBoxLayout* layout_;
};

//-----------------------------------------------------------------------------

#endif // RIBBONGROUPCONTAINER_H
