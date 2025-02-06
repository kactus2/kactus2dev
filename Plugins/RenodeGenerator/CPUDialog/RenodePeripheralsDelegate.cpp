//-----------------------------------------------------------------------------
// File: RenodePeripheralsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.03.2023
//
// Description:
// The delegate that provides editors to edit Renode peripheral details.
//-----------------------------------------------------------------------------

#include "RenodePeripheralsDelegate.h"

#include <Plugins/RenodeGenerator/CPUDialog/RenodeColumns.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeConstants.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>

#include <common/KactusColors.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QComboBox>
#include <QDir>
#include <QCoreApplication>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::RenodePeripheralsDelegate()
//-----------------------------------------------------------------------------
RenodePeripheralsDelegate::RenodePeripheralsDelegate(QWidget* parentWidget):
QStyledItemDelegate(parentWidget),
parentWidget_(parentWidget)
{

}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::setupTemplates()
//-----------------------------------------------------------------------------
void RenodePeripheralsDelegate::setupTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > newTemplates)
{
    pythonTemplates_ = newTemplates;
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::onFolderChanged()
//-----------------------------------------------------------------------------
void RenodePeripheralsDelegate::onFolderChanged(QString const& newFolder)
{
    generatorTargetFolder_ = newFolder;
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* RenodePeripheralsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == PeripheralColumns::CLASS || index.column() == PeripheralColumns::TEMPLATE)
    {
        auto editor(new QComboBox(parent));
        return editor;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void RenodePeripheralsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == PeripheralColumns::CLASS || index.column() == PeripheralColumns::TEMPLATE)
    {
		auto comboEditor = dynamic_cast<QComboBox*>(editor);
        if (comboEditor)
        {
            if (index.column() == PeripheralColumns::CLASS)
            {
				comboEditor->setEditable(true);
                QString filePath = QCoreApplication::applicationDirPath() + "/assets/peripherals.txt";
				if (QFile peripheralClassFile(filePath); peripheralClassFile.open(QIODevice::ReadOnly))
				{
					QTextStream fileStream(&peripheralClassFile);
					while (!fileStream.atEnd())
					{
                        comboEditor->addItem(fileStream.readLine());
					}

					peripheralClassFile.close();
				}
            }
			else if (index.column() == PeripheralColumns::TEMPLATE)
			{
				QStringList templateNames = { "" };
				templateNames.append(RenodeUtilities::getTemplateNames(pythonTemplates_));
				comboEditor->addItems(templateNames);
			}

            QString currentItem = index.data(Qt::DisplayRole).toString();
			comboEditor->setCurrentIndex(comboEditor->findText(currentItem));
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool RenodePeripheralsDelegate::editorEvent(QEvent *event, QAbstractItemModel* model, QStyleOptionViewItem const& option,
    QModelIndex const& index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // Always reset the ad-hoc group modify flag.
    if (event->type() == QEvent::MouseButtonRelease)
    {
        booleanModify_ = false;
        if (index.column() == PeripheralColumns::FILEPATH)
        {
            return handleEditorEventForPath(event, model, option, index);
        }
    }

    // Make sure that the item is checkable.
	if (Qt::ItemFlags flags = model->flags(index); !(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
    {
        return false;
    }

    // Make sure that we have a check state.
    QVariant value = index.data(Qt::CheckStateRole);

    if (!value.isValid())
    {
        return false;
    }

	auto newState = static_cast<Qt::CheckState>(value.toInt());

    // Handle the mouse button events.
    if (event->type() == QEvent::MouseButtonPress)
    {
        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

        QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
            option.decorationSize,
            QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
            option.rect.width() - (2 * textMargin),
            option.rect.height()));

        if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
        {
            return false;
        }

        newState = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
        booleanModify_ = true;
        booleanState_ = newState;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        if (!booleanModify_ || static_cast<Qt::CheckState>(value.toInt()) == booleanState_)
        {
            return false;
        }

        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

        QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
            option.decorationSize,
            QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
            option.rect.width() - (2 * textMargin),
            option.rect.height()));

        if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
        {
            return false;
        }
        
        newState = booleanState_;
    }
    else if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
            static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return model->setData(index, newState, Qt::CheckStateRole);
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::paint()
//-----------------------------------------------------------------------------
void RenodePeripheralsDelegate::paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const
{
	QStyleOptionViewItem viewItemOption(option);

    if (index.column() == PeripheralColumns::INITABLE)
    {
        QColor rectangleColor = KactusColors::REGULAR_FIELD;

        QModelIndex classIndex = index.sibling(index.row(), PeripheralColumns::CLASS);
		if (auto peripheralClass = classIndex.data(Qt::DisplayRole).toString(); peripheralClass != RenodeConstants::PYTHONPERIPHERAL)
        {
            rectangleColor = KactusColors::DISABLED_FIELD;
        }

        painter->fillRect(option.rect, rectangleColor);

        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

        QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                            QSize(option.decorationSize.width() + 5, option.decorationSize.height()),
                                            QRect(option.rect.x() + textMargin, option.rect.y(),
                                                  option.rect.width() - (2 * textMargin), option.rect.height()));
        viewItemOption.rect = newRect;
    }

    QStyledItemDelegate::paint(painter, viewItemOption, index);
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::handleEditorEventForPath()
//-----------------------------------------------------------------------------
bool RenodePeripheralsDelegate::handleEditorEventForPath(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option, QModelIndex const& index)
{
    const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
    
    QRect rectangleIsinideCheckRect(option.rect.x() + (2 * textMargin),
        option.rect.y(),
        option.rect.width() - (2 * textMargin),
        option.rect.height());
    
    QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignLeft, option.decorationSize, rectangleIsinideCheckRect);
    if (checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
    {
        QString newPath = "";
        newPath = QFileDialog::getExistingDirectory(parentWidget_, tr("Open Directory"), generatorTargetFolder_,
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (!newPath.isEmpty())
        {
            QDir appDirectory(generatorTargetFolder_);
            newPath = appDirectory.relativeFilePath(newPath);
        }

        if (!newPath.isEmpty())
        {
            model->setData(index, newPath);
        }

        return true;
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
