//-----------------------------------------------------------------------------
// File: CatalogValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.02.2017
//
// Description:
// Validator for ipxact:catalog.
//-----------------------------------------------------------------------------

#include "CatalogValidator.h"

#include <library/LibraryInterface.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Catalog/IpxactFile.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: CatalogValidator::CatalogValidator()
//-----------------------------------------------------------------------------
CatalogValidator::CatalogValidator()
{

}


//-----------------------------------------------------------------------------
// Function: CatalogValidator::~CatalogValidator()
//-----------------------------------------------------------------------------
CatalogValidator::~CatalogValidator()
{

}

//-----------------------------------------------------------------------------
// Function: CatalogValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool CatalogValidator::validate(QSharedPointer<Catalog> catalog) const
{
    return catalog->getVlnv().isValid() &&
        validateFiles(catalog->getCatalogs()) &&
        validateFiles(catalog->getBusDefinitions()) &&
        validateFiles(catalog->getAbstractionDefinitions()) &&
        validateFiles(catalog->getComponents()) &&
        validateFiles(catalog->getAbstractors()) &&
        validateFiles(catalog->getDesigns()) &&
        validateFiles(catalog->getDesignConfigurations()) &&
        validateFiles(catalog->getGeneratorChains());
}

//-----------------------------------------------------------------------------
// Function: CatalogValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void CatalogValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Catalog> catalog) const
{
	// Must have a valid VLNV.
	if (!catalog->getVlnv().isValid())
	{
		errors.append(QObject::tr("The VLNV %1 is invalid.").arg(catalog->getVlnv().toString()));
	}

    QString context = QObject::tr("catalog within catalog %1").arg(catalog->getVlnv().toString());
    findErrorsInFiles(errors, catalog->getCatalogs(), context);

    context = QObject::tr("bus definition within catalog %1").arg(catalog->getVlnv().toString());
    findErrorsInFiles(errors, catalog->getBusDefinitions(), context);

    context = QObject::tr("abstraction definition within catalog %1").arg(catalog->getVlnv().toString());
    findErrorsInFiles(errors, catalog->getAbstractionDefinitions(), context);

    context = QObject::tr("component within catalog %1").arg(catalog->getVlnv().toString());
    findErrorsInFiles(errors, catalog->getComponents(), context);

    context = QObject::tr("abstractor within catalog %1").arg(catalog->getVlnv().toString());
    findErrorsInFiles(errors, catalog->getAbstractors(), context);
    
    context = QObject::tr("design within catalog %1").arg(catalog->getVlnv().toString());
    findErrorsInFiles(errors, catalog->getDesigns(), context);

    context = QObject::tr("design configuration within catalog %1").arg(catalog->getVlnv().toString());
    findErrorsInFiles(errors, catalog->getDesignConfigurations(), context);

    context = QObject::tr("generator chain within catalog %1").arg(catalog->getVlnv().toString());
    findErrorsInFiles(errors, catalog->getGeneratorChains(), context);
}

//-----------------------------------------------------------------------------
// Function: CatalogValidator::validateFiles()
//-----------------------------------------------------------------------------
bool CatalogValidator::validateFiles(QSharedPointer<QList<QSharedPointer<IpxactFile> > > files) const
{
    foreach (QSharedPointer<IpxactFile> file, *files)
    {
        if (!validateFile(file))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: CatalogValidator::validateFile()
//-----------------------------------------------------------------------------
bool CatalogValidator::validateFile(QSharedPointer<IpxactFile> file) const
{
    return file->getVlnv().isValid() && !file->getName().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: CatalogValidator::findErrorsInFile()
//-----------------------------------------------------------------------------
void CatalogValidator::findErrorsInFiles(QVector<QString>& errors, 
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > files, 
    QString const& context) const
{
    foreach (QSharedPointer<IpxactFile> file, *files)
    {
        file->getVlnv().isValid(errors, context);

        if (file->getName().isEmpty())
        {
            errors.append(QObject::tr("File name must not be empty for %1.").arg(context));
        }
    }
}
