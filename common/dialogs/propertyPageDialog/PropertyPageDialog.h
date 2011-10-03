//-----------------------------------------------------------------------------
// File: PropertyPageDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 8.6.2011
//
// Description:
// Generic property page dialog.
//-----------------------------------------------------------------------------

#ifndef PROPERTYPAGEDIALOG_H
#define PROPERTYPAGEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QListWidgetItem>
#include <QPushButton>

class PropertyPageView;

//-----------------------------------------------------------------------------
//! PropertyPageDialog class.
//-----------------------------------------------------------------------------
class PropertyPageDialog : public QDialog
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! ApplyMode enumeration.
    //-----------------------------------------------------------------------------
    enum ApplyMode
    {
        APPLY_CURRENT = 0,
        APPLY_ALL
    };

    /*!
     *  Constructor.
     *
     *      @param [in] listIconSize  The icon size in the contents list.
     *      @param [in] maxListWidth  The maximum width of the contents list.
     *      @param [in] itemSize      The size for the whole item (icon and text) in the contents list.
     *      @param [in] mode          Apply mode which specifies what pages are applied when
     *                                the user presses OK.
     *      @param [in] parent        The parent widget. Can be null.
     */
    PropertyPageDialog(QSize const& listIconSize, int maxListWidth,
                       QSize const& itemSize, ApplyMode mode, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~PropertyPageDialog();

    /*!
     *  Adds a new property page to the dialog.
     *
     *      @param [in] icon  The icon for the page.
     *      @param [in] text  The text description for the page.
     *      @param [in] view  The page view.
     */
    void addPage(QIcon const& icon, QString const& text, PropertyPageView* view);

public slots:
    /*!
     *  Changes the current page based on the selection in the contents list.
     *
     *      @param [in] current   The current list item selected in the contents list.
     *      @param [in] previous  The previously selected item in the contents list.
     */
    void changePage(QListWidgetItem* current, QListWidgetItem* previous);

    /*!
     *  Called when the user presses the OK button.
     */
    virtual void accept();

    /*!
     *  Called when the contents of the current page have changed.
     */
    void onContentChanged();

private:
    // Disable copying.
    PropertyPageDialog(PropertyPageDialog const& rhs);
    PropertyPageDialog& operator=(PropertyPageDialog const& rhs);

    enum
    {
        ICON_SPACING = 12,
        LAYOUT_SPACING = 12
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The contents list widget.
    QListWidget* contentsList_;

    //! The pages.
    QStackedWidget* pages_;

    //! OK button.
    QPushButton* btnOk_;

    //! Item size.
    QSize itemSize_;

    //! Apply mode
    ApplyMode mode_;
};

//-----------------------------------------------------------------------------

#endif // PROPERTYPAGEDIALOG_H
