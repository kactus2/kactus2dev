//-----------------------------------------------------------------------------
// File: ImageExporter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.04.2018
//
// Description:
// Exports screen shots of the desired areas as a .png or .svg files.
//-----------------------------------------------------------------------------

#ifndef IMAGEEXPORTER_H
#define IMAGEEXPORTER_H

class VLNV;

#include <QWidget>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
//! Exports screen shots of the desired areas as a .png or .svg files.
//-----------------------------------------------------------------------------
namespace ImageExporter
{
    /*!
     *  Export an image of the selected document.
     *
     *      @param [in] libraryPath         Library path of the selected document.
     *      @param [in] identifyingVLNV     VLNV of the component containing the selected document.
     *      @param [in] scene               Graphics scene of the selected document.
     *      @param [in] parent              Parent item for determining the export path.
     */
    bool exportImage(QString const& libraryPath, VLNV const& identifyingVLNV, QGraphicsScene* scene,
        QWidget* parent);
}

#endif // IMAGEEXPORTER_H
