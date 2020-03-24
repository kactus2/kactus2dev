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

/*! VisualizerItem is base class that provides interface to visualize IP-Xact models.
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

	//! The minimum and maximum bounds for visualizer items.
	enum Bounds {
		DEFAULT_HEIGHT = 40,    //! The height of a single item
		MIN_WIDTH = 90,			//! The minimum width of an item
		DEFAULT_WIDTH = 200,	//! The default width of a single item
		NAME_INDENTATION = 15,  //! How much space the name leaves for left corners
        CORNER_INDENTATION = 4
	};

	//! The font size to display text.
	enum FontSize {
		FONT_NAMESIZE = 10,		//! The size for name field
		FONT_CORNERSIZE = 10	//! The size for corner texts
	};

    //! The address label position.
    enum LabelPosition
    {
        LABELS_LEFT = 0,
        LABELS_RIGHT,
        LABELS_TOP
    };

	/*! The constructor.
	 *
	 * \param parent Pointer to the owner of this graphics item.
	 *
	*/
	VisualizerItem(QGraphicsItem* parent = 0);
	
	//! The destructor
	virtual ~VisualizerItem() = default;

    //! No copying
    VisualizerItem(const VisualizerItem& other) = delete;

    //! No assignment
    VisualizerItem& operator=(const VisualizerItem& other) = delete;

	//! Refresh the item and possible sub-items
	virtual void refresh() = 0;

	/*! Get the rectangle reserved by this item and it's sub-items recursively.
	 *
	 * \return QRectF which contains the rectangles of this item and sub-items and their
	 * children recursively.
	*/
	virtual QRectF itemTotalRect() const;

	/*! Get the displayed name of the object. 
     *
	 * \return QString containing the name
	*/
	virtual QString name() const;

	/*! Set the width for the item.
	 *
	 * \param width The new width of the item.
	 *
	*/
	virtual void setWidth(qreal width);

	/*! Get the width of the item.
	 *
	 * The width is dependent on the parents width.
	 *
	 * \return The width of the item and it's sub-items.
	*/
	virtual qreal itemTotalWidth() const;

	/*! Set the name position of the item.
	 *
	 * \param namePos The position to be used.
	 *
	*/
	void setNamePosition(const NameHorizontalAlign hPos, const NameVerticalAlign vPos);
    
    
    void setAddressPosition(VisualizerItem::LabelPosition position);

protected slots:

	/*! Set new positions for child items.
	 * 
	 * The base class implementation only updates the position of the name and corner texts.
	*/
	virtual void reorganizeChildren();

protected:

	/*! Set the display name of the item.
	 *
	 * \param name The name to display.
	 *
	*/
	virtual void setName(QString const& name);

	/*! Set text to the top left corner.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setLeftTopCorner(QString const& text);

	/*! Set text to the bottom left corner.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setLeftBottomCorner(QString const& text);

	//! Set the position for text in the top left corner.
    virtual void setLeftTopPosition(QGraphicsSimpleTextItem* label);

    //! Set the position for text in the bottom left corner.
    virtual void setLeftBottomPosition(QGraphicsSimpleTextItem* label);

    //! Set the position for text in the top right corner.
    virtual void setRightTopPosition(QGraphicsSimpleTextItem* label);

    //! Set the position for text in the bottom right corner.
    virtual void setRightBottomPosition(QGraphicsSimpleTextItem* label);

	/*! Get the minimum rectangle of the item.
	 *
	 * \return QRectF that limits the minimum size needed by this item.
	*/
	virtual QRectF minimumRect() const;

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
    QBrush defaultBrush();


private:

    //! Resizes text boxes according to ohters.
    void resizeLabels();
    
    /*! If text is longer than maxChars, cuts the lenght to maxChars is and appends "...".
     *
	 * \param text The text to clip.
     * \param maxChars Maximum number of characters to show.
     *
	 * \return The string with maximum lenght of maxChars.
	*/
    QString clipText(QString const& text, int maxChars) const;

    //! The name of the item.
    QString name_;

    //! Default brush for item coloring.
    QBrush defaultBrush_;

	//! Defines where the name label is positioned.
	NameHorizontalAlign nameHorizontalPos_;
    NameVerticalAlign nameVerticalPos_;

    LabelPosition addressPosition_;

	//! The label to display the name of the memory map.
    QGraphicsSimpleTextItem nameLabel_;

	//! The label to display text in the left top corner.
	QGraphicsSimpleTextItem topText_;

	//! The label to display text in the left bottom corner.
	QGraphicsSimpleTextItem bottomText_;

};

#endif // VISUALIZERITEM_H
