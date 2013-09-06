/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: visualizeritem.h
 *		Project: Kactus 2
 */

#ifndef VISUALIZERITEM_H
#define VISUALIZERITEM_H

#include <QBrush>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSimpleTextItem>
#include <QRectF>
#include <QObject>

/*! \brief VisualizerItem is base class that provides interface to visualize IP-Xact models.
 *
 */
class VisualizerItem : public QObject, public QGraphicsRectItem {
	Q_OBJECT

public:

    enum NameHorizontalAlign
    {
        NAME_LEFT_ALIGN = 0, //! Name is aligned to left.
        NAME_CENTERED,       //! Name is aligned to center.
        NAME_RIGHT_ALIGN     //! Name is aligned to right.
    };

    enum NameVerticalAlign
    {
        NAME_TOP = 0,       //! Name is aligned to the top of the item.
        NAME_MIDDLE,        //! Name is aligned to the center of the item.
        NAME_BOTTOM         //! Name is aligned to the bottom of the item.
    };

	//! \brief The minimum and maximum bounds for visualizer items.
	enum Bounds {
		DEFAULT_HEIGHT = 40,    //! The height of a single item
		MIN_WIDTH = 90,			//! The minimum width of an item
		DEFAULT_WIDTH = 200,	//! The default width of a single item
		NAME_INDENTATION = 20,  //! How much space the name leaves for left corners
        CORNER_INDENTATION = 4
	};

	//! \brief The font size to display text.
	enum FontSize {
		FONT_NAMESIZE = 10,		//! The size for name field
		FONT_CORNERSIZE = 10	//! The size for corner texts
	};

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of this graphics item.
	 *
	*/
	VisualizerItem(QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~VisualizerItem();

	//! \brief Refresh the item and possible sub-items
	virtual void refresh() = 0;

	/*! \brief Get the rectangle reserved by this item and it's sub-items recursively.
	 *
	 * \return QRectF which contains the rectangles of this item and sub-items and their
	 * children recursively.
	*/
	virtual QRectF itemTotalRect() const;

	/*! \brief Get the displayed name of the object. 
     *
	 * \return QString containing the name
	*/
	virtual QString getName() const;

	/*! \brief Set the width for the item.
	 *
	 * \param width The new width of the item.
	 *
	*/
	virtual void setWidth(qreal width);

	/*! \brief Get the width of the item.
	 *
	 * The width is dependent on the parents width.
	 *
	 * \return The width of the item and it's sub-items.
	*/
	virtual qreal itemTotalWidth() const;

	/*! \brief Set the name position of the item.
	 *
	 * \param namePos The position to be used.
	 *
	*/
	virtual void setNamePosition(const NameHorizontalAlign hPos, const NameVerticalAlign vPos);
    

protected:

	/*! \brief Set the display name of the item.
	 *
	 * \param name The name to display.
	 *
	*/
	virtual void setName(const QString& name);

	/*! \brief Set text to the top left corner.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setLeftTopCorner(const QString& text);

	/*! \brief Set text to the bottom left corner.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setLeftBottomCorner(const QString& text);

	/*! \brief Set text to the top right corner.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setRightTopCorner(const QString& text);

	/*! \brief Set text to the bottom right corner.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setRightBottomCorner(const QString& text);

	//! \brief Set the position for text in the top left corner.
    virtual void setLeftTopPosition();

    //! \brief Set the position for text in the bottom left corner.
    virtual void setLeftBottomPosition();

    //! \brief Set the position for text in the top right corner.
    virtual void setRightTopPosition();

    //! \brief Set the position for text in the bottom right corner.
    virtual void setRightBottomPosition();

    //! \brief Set the position for name text.
    virtual void setNameLabelPosition();

	/*! \brief Get the minimum rectangle of the item.
	 *
	 * \return QRectF that limits the minimum size needed by this item.
	*/
	virtual QRectF minimumRect() const;

	/*! \brief Set new positions for child items.
	 * 
	 * The base class implementation only updates the position of the name and corner texts.
	*/
	virtual void reorganizeChildren();

    /*!
     *  Sets the default coloring brush for the item.
     *
     *      @param [in] brush   The brush to set.
     */
    virtual void setDefaultBrush(QBrush brush);

    /*!
     *  Gets the default coloring brush for the item.
     *
     *      @return [in] brush   The default brush.
     */
    virtual QBrush defaultBrush();

	//! Default brush for item coloring.
    QBrush defaultBrush_;  

private:
	
	//! \brief No copying
	VisualizerItem(const VisualizerItem& other);

	//! \brief No assignment
	VisualizerItem& operator=(const VisualizerItem& other);

    //! \brief Resizes text boxes according to ohters.
    void resizeLabels();
    
    /*! \brief If text is longer than maxChars, cuts the lenght to maxChars is and appends "...".
     *
	 * \param text The text to clip.
     * \param maxChars Maximum number of characters to show.
     *
	 * \return The string with maximum lenght of maxChars.
	*/
    QString clipText(QString const& text, int maxChars) const;

    //! \brief The name of the item.
    QString name_;

	//! \brief Defines where the name label is positioned.
	NameHorizontalAlign nameHorizontalPos_;
    NameVerticalAlign nameVerticalPos_;

	//! \brief The label to display the name of the memory map.
	QGraphicsTextItem nameLabel_;

	//! \brief The label to display text in the left top corner.
	QGraphicsSimpleTextItem leftTopText_;

	//! \brief The label to display text in the left bottom corner.
	QGraphicsSimpleTextItem leftBottomText_;

	//! \brief The label to display text in the right top corner.
	QGraphicsSimpleTextItem rightTopText_;

	//! \brief The label to display text in the right bottom corner.
	QGraphicsSimpleTextItem rightBottomText_;
};

#endif // VISUALIZERITEM_H
