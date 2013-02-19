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

//-----------------------------------------------------------------------------
// Function: PropertyPageDialog()
//-----------------------------------------------------------------------------
PropertyPageDialog::PropertyPageDialog(QSize const& listIconSize, int iconColumnCount,
                                       ViewMode viewMode, ApplyMode applyMode, QWidget* parent)
    : QDialog(parent),
      iconColumnCount_(iconColumnCount),
      contentsList_(0),
      pages_(0),
      btnOk_(0),
      viewMode_(viewMode),
      applyMode_(applyMode)
{
    // Create the contents list.
    contentsList_ = new QListWidget(this);
    contentsList_->setIconSize(listIconSize);

    if (viewMode == VIEW_ICONS)
    {
        contentsList_->setViewMode(QListView::IconMode);
        contentsList_->setMovement(QListView::Static);

        if (iconColumnCount == 1)
        {
            contentsList_->setWrapping(false);
        }

        contentsList_->setSpacing(ICON_SPACING);
        contentsList_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        contentsList_->setFlow(QListView::TopToBottom);
    }
    else
    {
        contentsList_->setViewMode(QListView::ListMode);
    }
    
    // Create the stacked pages widget and a layout with a separator (group box).
    pages_ = new QStackedWidget(this);

    QGroupBox* separator = new QGroupBox(this);
    separator->setFlat(true);

    QVBoxLayout* pageLayout = new QVBoxLayout();
    pageLayout->addWidget(pages_);
    pageLayout->addStretch(1);
    pageLayout->addWidget(separator);

    // Create the layout for the contents list and pages.
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(contentsList_);
    horizontalLayout->addLayout(pageLayout, 1);

    // Create the buttons and their layout.
    btnOk_ = new QPushButton(tr("OK") , this);
    QPushButton* btnCancel = new QPushButton(tr("Cancel"), this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnOk_);
    buttonLayout->addWidget(btnCancel);

    // Create the main layout.
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(horizontalLayout, 1);
    mainLayout->addLayout(buttonLayout);

    connect(btnOk_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(contentsList_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*, QListWidgetItem*)));
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
        QFontMetrics metrics(contentsList_->font());
        int textHeight = metrics.height();

        for (int i = 0; i < contentsList_->count(); ++i)
        {
            contentsList_->item(i)->setSizeHint(QSize(optimalWidth, contentsList_->sizeHintForRow(i) + textHeight));
        }

        contentsList_->setFixedWidth(optimalWidth * iconColumnCount_);
    }
    else
    {
        contentsList_->setFixedWidth(optimalWidth + 30);
    }
}
