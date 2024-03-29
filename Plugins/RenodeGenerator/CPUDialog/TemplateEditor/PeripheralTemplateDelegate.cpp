//-----------------------------------------------------------------------------
// File: PeripheralTemplateDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 31.05.2023
//
// Description:
// The delegate that provides editors to edit peripheral template details.
//-----------------------------------------------------------------------------

#include "PeripheralTemplateDelegate.h"

#include <Plugins/RenodeGenerator/CPUDialog/RenodeColumns.h>

#include <QFileDialog>
#include <QApplication>
#include <QMouseEvent>

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateDelegate::PeripheralTemplateDelegate()
//-----------------------------------------------------------------------------
PeripheralTemplateDelegate::PeripheralTemplateDelegate(QString const& generationDestinationFolder,
    QWidget* parentWidget, QObject* parent):
QStyledItemDelegate(parent),
generatorTargetFolder_(generationDestinationFolder),
parentWidget_(parentWidget)
{

}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool PeripheralTemplateDelegate::editorEvent(QEvent *event, QAbstractItemModel* model, QStyleOptionViewItem const& option,
    QModelIndex const& index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // Handle the mouse button events.
    if (event->type() == QEvent::MouseButtonRelease &&
        (index.column() == PeripheralTemplateColumns::PATH || index.column() == PeripheralTemplateColumns::TEMPLATE))
    {
        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

        QStyleOptionViewItem::Position iconPosition = option.decorationPosition;
        int iconWidth = option.decorationSize.width();

        QRect rectangleIsinideCheckRect(option.rect.x() + (2 * textMargin), option.rect.y(),
            option.rect.width() - (2 * textMargin),
            option.rect.height());

		QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignLeft, option.decorationSize, rectangleIsinideCheckRect);
        if (checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
        {
            QString newPath = "";
            if (index.column() == PeripheralTemplateColumns::PATH)
            {
				newPath = QFileDialog::getExistingDirectory(parentWidget_, tr("Open Directory"), generatorTargetFolder_,
					QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

                if (!newPath.isEmpty())
                {
                    QDir appDirectory(generatorTargetFolder_);
                    newPath = appDirectory.relativeFilePath(newPath);
                }
            }
            else if (index.column() == PeripheralTemplateColumns::TEMPLATE)
            {
                QString binaryFolder = QCoreApplication::applicationDirPath();

                newPath = QFileDialog::getOpenFileName(parentWidget_, tr("Select python template"), binaryFolder, tr("Python files (*.py)"));

                if (!newPath.isEmpty())
                {
                    QDir appDirectory(binaryFolder);
                    newPath = appDirectory.relativeFilePath(newPath);
                }
            }

            if (!newPath.isEmpty())
            {
                model->setData(index, newPath);
            }

            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
