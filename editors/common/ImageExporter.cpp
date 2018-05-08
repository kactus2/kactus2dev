//-----------------------------------------------------------------------------
// File: ImageExporter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.04.2018
//
// Description:
// Exports screen shots of the desired areas as a .png or .svg files.
//-----------------------------------------------------------------------------

#include "ImageExporter.h"

#include <IPXACTmodels/common/VLNV.h>

#include <QFileDialog>
#include <QSvgGenerator>
#include <QObject>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: ImageExporter::exportImage()
//-----------------------------------------------------------------------------
bool ImageExporter::exportImage(QString const& libraryPath, VLNV const& identifyingVLNV, QGraphicsScene* scene,
    QWidget* parent)
{
    QString exportPath = QFileDialog::getSaveFileName(parent, QObject::tr("Save image to file"), libraryPath,
        QObject::tr("SVG image (*.svg);;JPG image (*.jpg);;PNG image (*.png)"));
    QString fileExtension = exportPath.right(3);

    if (exportPath.isEmpty() && !fileExtension.isEmpty() &&
        (fileExtension.compare("SVG", Qt::CaseInsensitive) == 0 ||
        fileExtension.compare("PNG", Qt::CaseInsensitive) == 0 ||
        fileExtension.compare("JPG", Qt::CaseInsensitive) == 0))
    {
        return false;
    }

    QRectF boundingRect = scene->itemsBoundingRect();
    boundingRect.setHeight(boundingRect.height() + 2);
    boundingRect.setWidth(boundingRect.width() + 2);

    if (fileExtension.compare(QStringLiteral("SVG"), Qt::CaseInsensitive) == 0)
    {
        QString description = QString(QObject::tr("Memory design of ")) + identifyingVLNV.toString();

        QSvgGenerator generator;
        generator.setFileName(exportPath);
        generator.setSize(boundingRect.size().toSize());
        generator.setViewBox(boundingRect);
        generator.setTitle(QObject::tr("Memory Design"));
        generator.setDescription(description);

        QPainter painter;
        painter.begin(&generator);

        scene->render(&painter);

        painter.end();
    }
    else
    {
        QPixmap memoryImage(boundingRect.size().toSize());
        QPainter picPainter(&memoryImage);
        picPainter.fillRect(memoryImage.rect(), QBrush(Qt::white));
        scene->render(&picPainter, memoryImage.rect(), boundingRect.toRect());
        
        QFile memoryImageFile(exportPath);
        memoryImageFile.open(QIODevice::WriteOnly);
        memoryImage.save(&memoryImageFile);
    }

    return true;
}
