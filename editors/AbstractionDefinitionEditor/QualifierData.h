//-----------------------------------------------------------------------------
// File: QualifierData.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 26.06.2023
//
// Description:
// Data structure containing 2022 standard Abstraction Definition qualifier data.
//-----------------------------------------------------------------------------

#ifndef QUALIFIERDATA_H
#define QUALIFIERDATA_H

#include <QStringList>
#include <QMap>
#include <QMetaType>

//-----------------------------------------------------------------------------
//! Data structure containing 2022 standard Abstraction Definition qualifier data.
//-----------------------------------------------------------------------------
struct QualifierData
{
    //! The currently set qualifiers
    QStringList activeQualifiers_;

    //! The set qualifier attributes
    QMap<QString, QString> attributes_;
};

//! Declare metatype for use in QVariant
Q_DECLARE_METATYPE(QualifierData);

#endif // QUALIFIERDATA_H