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

#ifndef RIBBON_H
#define RIBBON_H

#include <QWidget>
#include <QColor>
#include <QHBoxLayout>
#include <QScrollArea>

#include <QPushButton>

class RibbonGroup;

//-----------------------------------------------------------------------------
//! Ribbon toolbar.
//-----------------------------------------------------------------------------
class Ribbon : public QWidget
{
	Q_OBJECT

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

public slots:

	/*!
	 *  Scrolls the scroll area left by the length of the next group.
	 */
	void onLeftScrollPressed();


	/*!
	 *  Scrolls the scroll area left by the length of the next group.
	 */
	void onRightScrollPressed();

protected:
    //! Draws the ribbon background.
    virtual void paintEvent(QPaintEvent* event);

	/*!
	 *  Gets the mouse wheel events and scrolls the horizontal bar..
	 *
	 *      @param [in] event   The mouse wheel event.
	 */
	virtual void wheelEvent(QWheelEvent* event);

	/*!
	 *  Resizes the widget and places scroll buttons if needed.
	 *
	 *      @param [in] event   The resize event.
	 */
	virtual void resizeEvent(QResizeEvent* event);

	/*!
	 *  Reimplemented show event to set the visibility of the scroll buttons.
	 *
	 *      @param [in] event   The show event.
	 */
	virtual void showEvent(QShowEvent* event);

	/*!
	 *  Event filter for the ribbon groups.
	 *
	 *      @param [in] object   The object in question.
	 *      @param [in] event   The current event.
	 */
	virtual bool eventFilter(QObject* object, QEvent* event);

private:
    // Disable copying.
    Ribbon(Ribbon const& rhs);
	Ribbon& operator=(Ribbon const& rhs);

	/*!
	 *  Setup the scroll area for the tool bar ribbon.
	 */
	void setupRibbonScrollArea();

	/*!
	 *  Setup the scroll buttons.
	 */
	void setupScrollButtons();

	/*!
	 *  Gives the amount the horizontal bar must be scrolled to the left.
	 */
	int getLeftScrollMovement();

	/*!
	 *  Gives the amount the horizontal bar must be scrolled to the right.
	 */
	int getRightScrollMovement();

	/*!
	 *  Sets the visibility of the left and right scroll buttons.
	 */
	void setScrollButtonVisibilities();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The layout for the ribbon groups.
    QHBoxLayout* layout_;

	//! The scroll area for the ribbon.
	QScrollArea* ribbonScrollArea_;

	//! Container widget for the ribbon groups.
	QWidget* ribbonGroupContainer_;

	//! Layout for the container widget.
	QHBoxLayout* containerLayout_;

	//! Button for scrolling left.
	QPushButton* scrollLeft_;

	//! Button for scrolling right.
	QPushButton* scrollRight_;

	//! Spacing amount for the container.
	static const int CONTAINER_SPACING = 3;

	//! The height of the ribbon.
	static const int RIBBONHEIGHT = 78;
};

//-----------------------------------------------------------------------------

#endif // RIBBON_H
