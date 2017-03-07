//-----------------------------------------------------------------------------
// File: PropertyPageDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 8.6.2011
//
// Description:
// Generic property page dialog.
//-----------------------------------------------------------------------------

#include "PropertyPageDialog.h"

#include "PropertyPageView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>

//-----------------------------------------------------------------------------
// Function: PropertyPageDialog()
//-----------------------------------------------------------------------------
PropertyPageDialog::PropertyPageDialog(QSize const& listIconSize, int iconColumnCount,
    ViewMode viewMode, ApplyMode applyMode, QWidget* parent):
QDialog(parent),
    iconColumnCount_(iconColumnCount),
    contentsList_(new QListWidget(this)),
    pages_(new QStackedWidget(this)),
    btnOk_(new QPushButton(tr("OK") , this)),
    viewMode_(viewMode),
    applyMode_(applyMode)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    contentsList_->setIconSize(listIconSize);

    if (viewMode == VIEW_ICONS)
    {
        contentsList_->setViewMode(QListView::IconMode);
        contentsList_->setMovement(QListView::Static);
        
        contentsList_->setUniformItemSizes(true);
        contentsList_->setSelectionRectVisible(true);

        contentsList_->setWrapping(iconColumnCount != 1);

        contentsList_->setSpacing(ICON_SPACING);
        contentsList_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        contentsList_->setFlow(QListView::TopToBottom);
    }
    else
    {
        contentsList_->setViewMode(QListView::ListMode);
    }
    
    setupLayout();
   
    connect(contentsList_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*, QListWidgetItem*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ~PropertyPageDialog()
//-----------------------------------------------------------------------------
PropertyPageDialog::~PropertyPageDialog()
{
}

//-----------------------------------------------------------------------------
// Function: addPage()
//-----------------------------------------------------------------------------
void PropertyPageDialog::addPage(QIcon const& icon, QString const& text, PropertyPageView* view)
{
    // Create the list widget item for the page.
    QListWidgetItem* listItem = new QListWidgetItem(contentsList_);
    listItem->setIcon(icon);
    listItem->setText(text);
    
    if (viewMode_ == VIEW_ICONS)
    {
        listItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    }

    listItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QFont font;
    font.setPixelSize(11);
    listItem->setFont(font);

    // Add the page to the stacked pages.
    pages_->addWidget(view);
    contentsList_->setCurrentRow(0);

    // Connect the contentChanged() signal.
    connect(view, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
}

//-----------------------------------------------------------------------------
// Function: changePage()
//-----------------------------------------------------------------------------
void PropertyPageDialog::changePage(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (current == 0)
    {
        current = previous;
    }

    // Call onPageChange() for the previous page.
    if (previous != 0)
    {
        int prevIndex = contentsList_->row(previous);
        PropertyPageView* prevPage = static_cast<PropertyPageView*>(pages_->widget(prevIndex));
        Q_ASSERT(prevPage != 0);

        // Reject the page change if the previous page disallows it.
        if (!prevPage->onPageChange())
        {
            contentsList_->setCurrentItem(previous);
            return;
        }
    }

    pages_->setCurrentIndex(contentsList_->row(current));
    onContentChanged();
}

//-----------------------------------------------------------------------------
// Function: accept()
//-----------------------------------------------------------------------------
void PropertyPageDialog::accept()
{
    if (applyMode_ == APPLY_CURRENT)
    {
        Q_ASSERT(pages_->currentWidget() != 0);
        PropertyPageView* page = static_cast<PropertyPageView*>(pages_->currentWidget());

        // Validate the contents of the page.
        if (!page->validate())
        {
            return;
        }

        QDialog::accept();

        // Apply the page.
        page->apply();
    }
    else if (applyMode_ == APPLY_ALL)
    {
        // First validate all pages.
        for (int i = 0; i < pages_->count(); ++i)
        {
            PropertyPageView* page = static_cast<PropertyPageView*>(pages_->widget(i));

            if (!page->validate())
            {
                return;
            }
        }

        QDialog::accept();

        // Then apply all pages.
        for (int i = 0; i < pages_->count(); ++i)
        {
            PropertyPageView* page = static_cast<PropertyPageView*>(pages_->widget(i));
            page->apply();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: onContentChanged()
//-----------------------------------------------------------------------------
void PropertyPageDialog::onContentChanged()
{
    Q_ASSERT(pages_->currentWidget() != 0);

    // Prevalidate the page to enable/disable the OK button.
    PropertyPageView* page = static_cast<PropertyPageView*>(pages_->currentWidget());
    btnOk_->setEnabled(page->prevalidate());
}

//-----------------------------------------------------------------------------
// Function: finalizePages()
//-----------------------------------------------------------------------------
void PropertyPageDialog::finalizePages()
{
    // Determine the best width for the list items.
    int optimalWidth = contentsList_->sizeHintForColumn(0);    

    // Update the width of the list.
    if (viewMode_ == VIEW_ICONS)
    {
        optimalWidth += 40;

        QFontMetrics metrics(contentsList_->font());
        int textHeight = metrics.height();
        int optimalHeight = contentsList_->iconSize().height() + textHeight + 4;
                
        for (int i = 0; i < contentsList_->count(); ++i)
        {
            contentsList_->item(i)->setSizeHint(QSize(optimalWidth, optimalHeight));
        }

        contentsList_->setFixedHeight((optimalHeight + ICON_SPACING) * contentsList_->count() + ICON_SPACING);
        contentsList_->setFixedWidth(optimalWidth * iconColumnCount_);
    }
    else
    {
        contentsList_->setFixedWidth(optimalWidth + 30);
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyPageDialog::setupLayout()
//-----------------------------------------------------------------------------
void PropertyPageDialog::setupLayout()
{
    QGroupBox* separator = new QGroupBox(this);
    separator->setFlat(true);

    QDialogButtonBox* buttons = new QDialogButtonBox(this);
    buttons->addButton(btnOk_, QDialogButtonBox::AcceptRole);
    buttons->addButton(QDialogButtonBox::Cancel);

    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

    QGridLayout* topLayout = new QGridLayout(this);
    topLayout->addWidget(contentsList_, 0, 0, 2, 1);
    topLayout->addWidget(pages_, 0, 1, 1, 1);
    topLayout->addWidget(separator, 1, 1, 1, 1);
    topLayout->addWidget(buttons, 2, 0, 1, 2);

    //topLayout->setRowStretch(0, 10);
}
