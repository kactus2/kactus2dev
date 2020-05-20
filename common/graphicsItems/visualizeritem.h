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
    enum LabelLayout
    {
        LABELS_LEFT = 0,
        LABELS_RIGHT,
        LABELS_TOP,
        UNDEFINED
    };

	/*! The constructor.
	 *
	 *      @param parent Pointer to the owner of this graphics item.
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
    virtual void refresh(){ };

	/*! Get the rectangle reserved by this item and it's sub-items recursively.
	 *
	 *      @return The rectangle of this item and sub-items and their children recursively.
	*/
	virtual QRectF itemTotalRect() const;

	/*! Get the displayed name of the object. 
     *
	 *      @return The name of the object.
	*/
	virtual QString name() const;

	/*! Set the width for the item.
	 *
	 *      @param width The new width of the item.
	 *
	*/
	virtual void setWidth(qreal width);

	/*! Get the width of the item.
	 *
	 * The width is dependent on the parents width.
	 *
	 *      @return The width of the item and it's sub-items.
	*/
	virtual qreal itemTotalWidth() const;
 
protected slots:

	/*! Set new positions for child items.
	 * 
	 * The base class implementation only updates the position of the name and corner texts.
	*/
	virtual void reorganizeChildren();

protected:

	/*! Set the display name of the item.
	 *
	 *      @param name The name to display.
	 *
	*/
	virtual void setName(QString const& name);

	/*! Set text to the top left corner.
	 *
	 *      @param text The text to display in the corner.
	 *
	*/
	virtual void setTopLabelText(QString const& text);

	/*! Set text to the bottom left corner.
	 *
	 *      @param text The text to display in the corner.
	 *
	*/
	virtual void setBottomLabelText(QString const& text);

    /*!
     * Set how the text labels are positioned within the item.
     *
     *     @param [in] labelLayout  The positioning style to set.     
     */
    void setLayoutType(VisualizerItem::LabelLayout labelLayout);

    /*!
     * Set the text labels contents to be shortened, if the label cannot fit within the item.
     *
     *     @param [in] enableClip  If true, text is shortened, otherwise not.
     */
     void setClipText(bool enableClip);


	/*! Get the minimum rectangle of the item.
	 *
	 *      @return The minimum size needed by this item.
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
    void setLabelPositions();

    //! Set the position for text in the top left corner.
    void setLeftTopPosition(QGraphicsSimpleTextItem* label);

    //! Set the position for text in the bottom left corner.
    void setLeftBottomPosition(QGraphicsSimpleTextItem* label);

    //! Set the position for text in the top right corner.
    void setRightTopPosition(QGraphicsSimpleTextItem* label);

    //! Set the position for text in the bottom right corner.
    void setRightBottomPosition(QGraphicsSimpleTextItem* label);

    void clipName();

    void clipAddress(QString const& text, QGraphicsSimpleTextItem* item);

    /*! If text is longer than maxChars, cuts the length to maxChars is and appends "...".
     *
	 *      @param text The text to clip.
     *      @param maxChars Maximum number of characters to show.
     *
	 *      @return The string with maximum length of maxChars.
	*/
    QString clipText(QString const& text, int maxChars) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the item.
    QString name_;

    //! The text displayed at the top label.
    QString topText_;

    //! The text displayed at the bottom label.
    QString bottomText_;
    
    //! Enable for text clipping i.e. shorten, if the text cannot fit in the given space.
    bool clipText_ = false;

    //! Default brush for item coloring.
    QBrush defaultBrush_;

    //! The layout style for labels.
    LabelLayout labelPositioning_;

	//! The label to display the name of the memory map.
    QGraphicsSimpleTextItem nameItem_;

	//! The label to display text in the left top corner.
	QGraphicsSimpleTextItem topTextItem_;

	//! The label to display text in the left bottom corner.
	QGraphicsSimpleTextItem bottomTextItem_;

};

#endif // VISUALIZERITEM_H
