//-----------------------------------------------------------------------------
// File: librarydata.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// LibraryData is the data model that manages the actual VLNV library.
//-----------------------------------------------------------------------------

#include "librarydata.h"

#include "libraryhandler.h"
#include "librarytreemodel.h"

#include <common/utils.h>
#include <common/widgets/ScanProgressWidget/scanprogresswidget.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinitionReader.h>
#include <IPXACTmodels/AbstractionDefinition/validators/AbstractionDefinitionValidator.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinitionReader.h>
#include <IPXACTmodels/BusDefinition/validators/BusDefinitionValidator.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/DesignReader.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/designConfiguration/DesignConfigurationReader.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/ComponentReader.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/kactusExtensions/ComDefinitionReader.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinitionReader.h>

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QList>
#include <QObject>
#include <QSettings>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QWidget>

//-----------------------------------------------------------------------------
// Function: librarydata::()
//-----------------------------------------------------------------------------
LibraryData::LibraryData(LibraryHandler* parent, QWidget* parentWidget):
QObject(parent),
parentWidget_(parentWidget),
libraryItems_(),
handler_(parent),
progWidget_(0),
timerSteps_(0),
timerStep_(0),
timer_(0),
locations_(),
iterObjects_(),
errors_(0),
failedObjects_(0),
syntaxErrors_(0),
vlnvErrors_(0),
fileErrors_(0),
fileCount_(0),
urlTester_(new QRegExpValidator(Utils::URL_VALIDITY_REG_EXP, this)),
componentValidatorFinder_(new ComponentParameterFinder(QSharedPointer<Component>())),
componentValidator_(QSharedPointer<ExpressionParser>(new IPXactSystemVerilogParser(componentValidatorFinder_)),
                    handler_),
designValidator_(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()), handler_),
designConfigurationValidator_(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()), handler_),
systemDesignConfigurationValidator_(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()), handler_)
{
	connect(this, SIGNAL(errorMessage(QString const&)),
        parent, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(QString const&)),
		parent, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LibraryData::~LibraryData()
//-----------------------------------------------------------------------------
LibraryData::~LibraryData()
{
	libraryItems_.clear();
}

//-----------------------------------------------------------------------------
// Function: LibraryData::getItems()
//-----------------------------------------------------------------------------
QList<VLNV> LibraryData::getItems() const
{
    return libraryItems_.keys();
}

//-----------------------------------------------------------------------------
// Function: LibraryData::getPath()
//-----------------------------------------------------------------------------
QString LibraryData::getPath(VLNV const& vlnv)
{
	if (libraryItems_.contains(vlnv))
    {
		return libraryItems_.value(vlnv);
	}
	else
    {
		emit errorMessage(tr("The VLNV \n"
			"Vendor: %1\n"
			"Library: %2\n"
			"Name: %3\n"
			"Version: %4\n"
			"was not found in the library.").arg(
            vlnv.getVendor(), vlnv.getLibrary(), vlnv.getName(), vlnv.getVersion()));

		return QString();
	}
}

//-----------------------------------------------------------------------------
// Function: LibraryData::addVLNV()
//-----------------------------------------------------------------------------
bool LibraryData::addVLNV(VLNV const& vlnv, QString const& path)
{
	if (libraryItems_.contains(vlnv))
    {
		emit errorMessage(tr("The VLNV \n"
			"Vendor: %1\n"
			"Library: %2\n"
			"Name: %3\n"
			"Version: %4\n"
			"Already existed in the library and was not added.").arg(
			vlnv.getVendor(), vlnv.getLibrary(), vlnv.getName(), vlnv.getVersion()));

		return false;
	}

	QFileInfo fileInfo(path);
	if (!fileInfo.exists())
    {
		emit errorMessage(tr("The file %1 was not found in file system").arg(path));
		return false;
	}

	// add the component to the library
	libraryItems_.insert(vlnv, path);

	emit addVLNV(vlnv);

	return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::contains()
//-----------------------------------------------------------------------------
bool LibraryData::contains(VLNV const& vlnv)
{
	return libraryItems_.contains(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryData::onRemoveVLNV()
//-----------------------------------------------------------------------------
void LibraryData::onRemoveVLNV(VLNV const& vlnv)
{
	if (!libraryItems_.contains(vlnv))
    {
		return;
	}

    libraryItems_.remove(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryData::getType()
//-----------------------------------------------------------------------------
VLNV::IPXactType LibraryData::getType(VLNV const& vlnv) const
{
	if (!libraryItems_.contains(vlnv))
    {
		return VLNV::INVALID;
	}
	else
    {
		return libraryItems_.find(vlnv).key().getType();
	}
}

//-----------------------------------------------------------------------------
// Function: LibraryData::resetLibrary()
//-----------------------------------------------------------------------------
void LibraryData::resetLibrary()
{
	emit resetModel();
}

//-----------------------------------------------------------------------------
// Function: LibraryData::checkLibraryIntegrity()
//-----------------------------------------------------------------------------
void LibraryData::checkLibraryIntegrity()
{
	int max = libraryItems_.size();
	errors_ = 0;
	failedObjects_ = 0;
	syntaxErrors_ = 0;
	vlnvErrors_ = 0;
	fileErrors_ = 0;
	fileCount_ = 0;

	emit noticeMessage(tr("------ Library Integrity Check ------"));

    timerStep_ = 0;
    timerSteps_ = max;
    iterObjects_ = libraryItems_.begin();

    // Create the progress bar that displays the progress of the check.
    progWidget_ = new ScanProgressWidget(parentWidget_);
    progWidget_->setWindowTitle(tr("Checking integrity..."));
    progWidget_->setRange(0, max);
    progWidget_->setMessage(tr("Processing item %1 of %2...").arg(QString::number(timerStep_ + 1),
        QString::number(libraryItems_.size())));
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(performIntegrityCheckStep()));
    timer_->start();

    progWidget_->exec();

	emit noticeMessage(tr("========== Library integrity check complete =========="));
	emit noticeMessage(tr("Total library object count: %1").arg(libraryItems_.size()));
	emit noticeMessage(tr("Total file count in the library: %1").arg(fileCount_));

	// if errors were found then print the summary of error types
	if (errors_ > 0)
    {
		emit noticeMessage(tr("Found %1 errors within %2 item(s):").arg(errors_).arg(failedObjects_));
		emit noticeMessage(tr("Structural errors within item(s): %1").arg(syntaxErrors_));
		emit noticeMessage(tr("Invalid VLNV references: %1").arg(vlnvErrors_));
		emit noticeMessage(tr("Invalid file references: %1\n").arg(fileErrors_));
	}
}

//-----------------------------------------------------------------------------
// Function: LibraryData::parseLibrary()
//-----------------------------------------------------------------------------
void LibraryData::parseLibrary()
{
	// clear the previous items in the library
	libraryItems_.clear();

	QSettings settings(this);

	locations_ = settings.value("Library/ActiveLocations", QStringList()).toStringList();
	if (!locations_.isEmpty()) 
    {
        // create the progress bar that displays the progress of the scan
        progWidget_ = new ScanProgressWidget(parentWidget_);
        progWidget_->setRange(0, locations_.size());
        progWidget_->setMessage("Scanning location: \n" + locations_.first());
        timerStep_ = 0;
        timerSteps_ = locations_.size();

        timer_ = new QTimer(this);
        connect(timer_, SIGNAL(timeout()), this, SLOT(performParseLibraryStep()));
        timer_->start();

        progWidget_->exec();
    }

    // check the integrity of the items in the library
	checkLibraryIntegrity();
}

//-----------------------------------------------------------------------------
// Function: LibraryData::parseDirectory()
//-----------------------------------------------------------------------------
void LibraryData::parseDirectory(QString const& directoryPath)
{
	if (directoryPath.isEmpty())
    {
		return;
	}

	QDir dirHandler(directoryPath);

	// get list of files and folders
	foreach (QFileInfo const& entryInfo, dirHandler.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files))
    {
		if (entryInfo.exists())
        {
            if (entryInfo.isFile() && entryInfo.suffix() == QLatin1String("xml"))
            {
                parseFile(entryInfo.absoluteFilePath());
            }
            else if (entryInfo.isDir())
            {
                parseDirectory(entryInfo.absoluteFilePath());
            }
		}
	}
}

//-----------------------------------------------------------------------------
// Function: LibraryData::parseFile()
//-----------------------------------------------------------------------------
void LibraryData::parseFile(QString const& filePath)
{
	if (filePath.isEmpty())
    {
		return;
	}

	QFile docFile(filePath);
	if (!docFile.open(QFile::ReadOnly))
    {
		emit errorMessage(tr("Could not open file %1 for reading.").arg(filePath));
		return;
	}
	
	VLNV vlnv;

	QDomDocument doc;
	if (doc.setContent(&docFile))
    {		
		docFile.close();
		vlnv = getDocumentVLNV(doc);
	}
	else // content could not be read.
    {
		emit errorMessage(tr("The file %1 was not valid xml and could not be read.").arg(filePath));
		docFile.close();
		return;
	}

	if (!vlnv.isValid())
    {
		return;
	}
	
	if (libraryItems_.contains(vlnv))
    {
		emit noticeMessage(tr("VLNV %1 was already found in the library").arg(vlnv.toString()));
		return;
	}

	// add the component to the library if parsing was successful
	libraryItems_.insert(vlnv, filePath);
}

//-----------------------------------------------------------------------------
// Function: LibraryData::getModel()
//-----------------------------------------------------------------------------
QSharedPointer<Document> LibraryData::getModel(VLNV const& vlnv)
{
	if (!libraryItems_.contains(vlnv))
    {
		//emit noticeMessage(tr("VLNV %1 was not found in the library").arg(vlnv.toString()));
		return QSharedPointer<Document>();
	}

	VLNV toCreate = vlnv;
	toCreate.setType(libraryItems_.find(vlnv).key().getType());
	
	QString path = libraryItems_.value(toCreate);
	if (path.isEmpty())
    {
		return QSharedPointer<Document>();
	}

	// create file handle and use it to read the IP-Xact document into memory
	QFile file(path);
	QDomDocument doc;
    if (!doc.setContent(&file))
    {
		emit errorMessage(tr("The document %1 in file %2 could not be opened.").arg(toCreate.toString(), path));
		return QSharedPointer<Document>();
    }
    file.close();

    // Create correct type of object.
    if (toCreate.getType() == VLNV::BUSDEFINITION)
    {
        BusDefinitionReader reader;
        return reader.createBusDefinitionFrom(doc);
    }
    else if (toCreate.getType() == VLNV::COMPONENT)
    {
        ComponentReader reader;
        return reader.createComponentFrom(doc);
    }
    else if (toCreate.getType() == VLNV::DESIGN)
	{
        DesignReader reader;
        return reader.createDesignFrom(doc);
    }
    else if (toCreate.getType() == VLNV::DESIGNCONFIGURATION)
    {
        DesignConfigurationReader reader;
        return reader.createDesignConfigurationFrom(doc);
    }
    else if (toCreate.getType() == VLNV::ABSTRACTIONDEFINITION)
    {
        AbstractionDefinitionReader reader;
        return reader.createAbstractionDefinitionFrom(doc);
    }
    else if (toCreate.getType() == VLNV::COMDEFINITION)
    {
		ComDefinitionReader reader;
		return reader.createComDefinitionFrom(doc);
    }
    else if (toCreate.getType() == VLNV::APIDEFINITION)
    {
		ApiDefinitionReader reader;
		return reader.createApiDefinitionFrom(doc);
    }
    else
    {
        emit noticeMessage(tr("Document was not supported type"));
        return QSharedPointer<Document>();
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::performParseLibrary()
//-----------------------------------------------------------------------------
void LibraryData::performParseLibraryStep()
{
	if (timerStep_ == timerSteps_)
	{
		return;
	}

	QString const& location = locations_.at(timerStep_);
	QFileInfo locationInfo(location);

	if (locationInfo.isDir())
	{
		parseDirectory(location);
	}
	else if (locationInfo.isFile())
	{
		parseFile(location);
	}

	timerStep_++;

	// Check if all steps have been completed.
	if (timerStep_ == timerSteps_)
	{
		timer_->stop();
		delete timer_;
		delete progWidget_;
	}
	else
	{
		// Update the value and message for the next round.
		progWidget_->setMessage("Scanning location: \n" + locations_.at(timerStep_));
		progWidget_->setValue(timerStep_ + 1);
	}
}

//-----------------------------------------------------------------------------
// Function: LibraryData::performIntegrityCheckStep()
//-----------------------------------------------------------------------------
void LibraryData::performIntegrityCheckStep()
{
	if (timerStep_ > timerSteps_)
	{
		return;
	}

	if (timerStep_ < timerSteps_)
	{
		QSharedPointer<Document> document = getModel(iterObjects_.key());
		// if the object could not be parsed
		if (!document)
        {
			// remove the pair from the map and move on
			QMap<VLNV, QString>::iterator i = libraryItems_.find(iterObjects_.key());
			iterObjects_++;
            libraryItems_.erase(i);
		}
        else
        {
            QVector<QString> errors = findErrorsInDocument(document, iterObjects_.value());
            if (!errors.isEmpty())
            {
                emit noticeMessage(tr("The following errors were found while processing item %1:").arg(
                    document->getVlnv().toString()));

				emit errorMessage(QStringList(errors.toList()).join('\n'));

                failedObjects_++;
            }
            iterObjects_++;
        }
	}
	else
	{
		// inform tree model that it needs to reset model also
		emit resetModel();
	}

	timerStep_++;

	// Update the message and value for the next round.
	if (timerStep_ < timerSteps_)
	{
		progWidget_->setMessage(tr("Processing item %1 of %2...").arg(QString::number(timerStep_ + 1),
			QString::number(libraryItems_.size())));
	}
	else
	{
		progWidget_->setMessage(tr("Updating library view..."));
	}

	progWidget_->setValue(timerStep_);

	// Check if all steps have been completed.
	if (timerStep_ > timerSteps_)
	{
		timer_->stop();
		delete timer_;
		timer_ = 0;

		delete progWidget_;
		progWidget_ = 0;
	}
}

//-----------------------------------------------------------------------------
// Function: LibraryData::validateDocument()
//-----------------------------------------------------------------------------
bool LibraryData::validateDocument(QSharedPointer<Document> document)
{
    Q_ASSERT(document);

	VLNV documentVLNV = document->getVlnv();
	QString documentPath = getPath(documentVLNV);

    Q_ASSERT(!documentPath.isEmpty());

    if (!QFileInfo(documentPath).exists())
    {
        return false;
    }

	if (getType(documentVLNV) == VLNV::BUSDEFINITION)
    {
        BusDefinitionValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));
        if (!validator.validate(document.dynamicCast<BusDefinition>()))
        {
            return false;
        }
    }
	else if (getType(documentVLNV) == VLNV::ABSTRACTIONDEFINITION)
    {
        AbstractionDefinitionValidator validator(handler_, QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));
        if (!validator.validate(document.dynamicCast<AbstractionDefinition>()))
        {
            return false;
        }
    }
	else if (getType(documentVLNV) == VLNV::COMPONENT)
    {
        QSharedPointer<Component> currentComponent = document.dynamicCast<Component>();

        changeComponentValidatorParameterFinder(currentComponent);

        if (currentComponent && !componentValidator_.validate(currentComponent))
        {
            return false;
        }
    }
    else if (getType(documentVLNV) == VLNV::DESIGN)
    {
        if (!designValidator_.validate(document.dynamicCast<Design>()))
        {
            return false;
        }
    }
    else if (getType(documentVLNV) == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<DesignConfiguration> configuration = document.dynamicCast<DesignConfiguration>();

        if (configuration->getImplementation() == KactusAttribute::SYSTEM)
        {
            if (!systemDesignConfigurationValidator_.validate(configuration))
            {
                return false;
            }
        }
        else
        {
            if (!designConfigurationValidator_.validate(configuration))
            {
                return false;
            }
        }
    }

    return validateDependentVLNVReferencences(document) &&
        validateDependentDirectories(document, documentPath) &&
        validateDependentFiles(document, documentPath);
}

//-----------------------------------------------------------------------------
// Function: librarydata::changeComponentValidatorParameterFinder()
//-----------------------------------------------------------------------------
void LibraryData::changeComponentValidatorParameterFinder(QSharedPointer<Component> targetComponent)
{
    componentValidatorFinder_->changeComponent(targetComponent);
}

//-----------------------------------------------------------------------------
// Function: LibraryData::findErrorsInDocument()
//-----------------------------------------------------------------------------
QVector<QString> LibraryData::findErrorsInDocument(QSharedPointer<Document> document, QString const& path)
{
    Q_ASSERT(document);

    QVector<QString> errorList;

	if (!QFileInfo(path).exists())
    {
        errorList.append(tr("The file %1 for the document was not found.").arg(path));
		errors_++;
	}

	// Check if the document xml is valid and if not then print errors of the document.
	if (getType(document->getVlnv()) == VLNV::BUSDEFINITION)
    {
        findErrorsInBusDefinition(document.dynamicCast<BusDefinition>(), errorList);
    }
    else if (getType(document->getVlnv()) == VLNV::ABSTRACTIONDEFINITION)
    {
        findErrorsInAbstractionDefinition(document.dynamicCast<AbstractionDefinition>(), errorList);
    }
    else if (getType(document->getVlnv()) == VLNV::COMPONENT)
    {
        findErrorsInComponent(document.dynamicCast<Component>(), errorList);
    }
    else if (getType(document->getVlnv()) == VLNV::DESIGN)
    {
        findErrorsInDesign(document.dynamicCast<Design>(), errorList);
    }
    else if (getType(document->getVlnv()) == VLNV::DESIGNCONFIGURATION)
    {
        findErrorsInDesignConfiguration(document.dynamicCast<DesignConfiguration>(), errorList);
    }

    findErrorsInDependentVLNVReferencences(document, errorList);

    findErrorsInDependentDirectories(document, path, errorList);

    findErrorsInDependentFiles(document, path, errorList);

	return errorList;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::findErrorsInBusDefinition()
//-----------------------------------------------------------------------------
void LibraryData::findErrorsInBusDefinition(QSharedPointer<BusDefinition> busDefinition,
    QVector<QString>& errorList)
{
    int errorsBeforeValidation = errorList.size();
    
    BusDefinitionValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));
    validator.findErrorsIn(errorList, busDefinition);

    int errorsInBusDefinition = errorList.size() - errorsBeforeValidation;

    if (errorsInBusDefinition != 0)
    {
        errors_ += errorsInBusDefinition;
        syntaxErrors_ += errorsInBusDefinition;
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::findErrorsInAbstractionDefinition()
//-----------------------------------------------------------------------------
void LibraryData::findErrorsInAbstractionDefinition(QSharedPointer<AbstractionDefinition> abstraction, QVector<QString>& errorList)
{
    int errorsBeforeValidation = errorList.size();

    AbstractionDefinitionValidator validator(handler_, 
        QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));
    validator.findErrorsIn(errorList, abstraction);

    int errorsInBusDefinition = errorList.size() - errorsBeforeValidation;

    if (errorsInBusDefinition != 0)
    {
        errors_ += errorsInBusDefinition;
        syntaxErrors_ += errorsInBusDefinition;
    }
}

//-----------------------------------------------------------------------------
// Function: librarydata::findErrorsInComponent()
//-----------------------------------------------------------------------------
void LibraryData::findErrorsInComponent(QSharedPointer<Component> component, QVector<QString>& errorList)
{
    int errorsBeforeValidation = errorList.size();

    changeComponentValidatorParameterFinder(component);

    componentValidator_.findErrorsIn(errorList, component);

    int errorsInComponent = errorList.size() - errorsBeforeValidation;
    if (errorsInComponent != 0)
    {
        errors_ += errorsInComponent;
        syntaxErrors_ += errorsInComponent;
    }
}

//-----------------------------------------------------------------------------
// Function: librarydata::findErrorsInDesign()
//-----------------------------------------------------------------------------
void LibraryData::findErrorsInDesign(QSharedPointer<Design> design, QVector<QString>& errorList)
{
    int errorsBeforeValidation = errorList.size();

    designValidator_.findErrorsIn(errorList, design);

    int errorsInDesign = errorList.size() - errorsBeforeValidation;
    if (errorsInDesign != 0)
    {
        errors_ += errorsInDesign;
        syntaxErrors_ += errorsInDesign;
    }
}

//-----------------------------------------------------------------------------
// Function: librarydata::findErrorsInDesignConfiguration()
//-----------------------------------------------------------------------------
void LibraryData::findErrorsInDesignConfiguration(QSharedPointer<DesignConfiguration> configuration,
    QVector<QString>& errorList)
{
    int errorsBeforeValidation = errorList.size();

    if (configuration->getImplementation() == KactusAttribute::SYSTEM)
    {
        systemDesignConfigurationValidator_.findErrorsIn(errorList, configuration);
    }
    else
    {
        designConfigurationValidator_.findErrorsIn(errorList, configuration);
    }

    int errorsInDesignConfiguration = errorList.size() - errorsBeforeValidation;
    if (errorsInDesignConfiguration != 0)
    {
        errors_ += errorsInDesignConfiguration;
        syntaxErrors_ += errorsInDesignConfiguration;
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::validateDependentVLNVReferencences()
//-----------------------------------------------------------------------------
bool LibraryData::validateDependentVLNVReferencences(QSharedPointer<Document> document)
{
    foreach (VLNV const& vlnv, document->getDependentVLNVs())
    {
        if (!libraryItems_.contains(vlnv))
        {
           return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::findErrorsInDependentVLNVReferencences()
//-----------------------------------------------------------------------------
void LibraryData::findErrorsInDependentVLNVReferencences(QSharedPointer<const Document> document,
    QVector<QString>& errorList)
{
    foreach (VLNV const& vlnv, document->getDependentVLNVs())
    {
        if (!libraryItems_.contains(vlnv))
        {
            errorList.append(tr("The referenced VLNV was not found in the library: %1").arg(vlnv.toString()));
            errors_++;
            vlnvErrors_++;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::validateDependentDirectories()
//-----------------------------------------------------------------------------
bool LibraryData::validateDependentDirectories(QSharedPointer<Document> document, QString const& documentPath)
{
    foreach (QString const& directory, document->getDependentDirs())
    {
        QString dirPath = General::getAbsolutePath(documentPath, directory);
        QFileInfo dirInfo(dirPath);

        if (!dirInfo.exists())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::findErrorsInDependentDirectories()
//-----------------------------------------------------------------------------
void LibraryData::findErrorsInDependentDirectories(QSharedPointer<const Document> document, 
    QString const& documentPath, QVector<QString>& errorList)
{
    foreach (QString const& directory, document->getDependentDirs())
    {
        QString dirPath = General::getAbsolutePath(documentPath, directory);

        if (!QFileInfo(dirPath).exists())
        {
            errorList.append(tr("\tDirectory %1 was not found in the file system.").arg(dirPath));
            errors_++;
            fileErrors_++;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::validateDependentFiles()
//-----------------------------------------------------------------------------
bool LibraryData::validateDependentFiles(QSharedPointer<Document> document, QString const& documentPath)
{
    foreach (QString filePath, document->getDependentFiles())
    {
        int pos = 0;
        if (urlTester_->validate(filePath, pos) != QValidator::Acceptable)
        {
            QString absolutePath = filePath;

            if (QFileInfo(filePath).isRelative())
            {
                absolutePath = General::getAbsolutePath(documentPath, filePath);
            }

            if (!QFileInfo(absolutePath).exists())
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::findErrorsInDependentFiles()
//-----------------------------------------------------------------------------
void LibraryData::findErrorsInDependentFiles(QSharedPointer<const Document> document, 
    QString const& documentPath, QVector<QString>& errorList)
{
    foreach (QString filePath, document->getDependentFiles())
    {
        // Check if the path is actually a URL to (external) location.
        int pos = 0;
        if (urlTester_->validate(filePath, pos) == QValidator::Acceptable)
        {
            fileCount_++;
        }

        // The path was not URL so it must be file reference on the disk.
        else 
        {
            QString absolutePath = filePath;

            if (QFileInfo(filePath).isRelative())
            {
                absolutePath = General::getAbsolutePath(documentPath, filePath);
            }

            if (QFileInfo(absolutePath).exists())
            {
                fileCount_++;
            }
            else
            {
                errorList.append(tr("File %1 was not found in the file system.").arg(filePath));               
                errors_++;
                fileErrors_++;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::getVLNV()
//-----------------------------------------------------------------------------
VLNV LibraryData::getDocumentVLNV(QDomDocument& doc)
{
    // get the type of the document
    QDomNodeList nodeList = doc.childNodes();

    QDomElement documentElement = doc.documentElement();
    QString type = documentElement.nodeName();

    QString vendor = documentElement.firstChildElement("ipxact:vendor").firstChild().nodeValue();
    QString library = documentElement.firstChildElement("ipxact:library").firstChild().nodeValue();
    QString name = documentElement.firstChildElement("ipxact:name").firstChild().nodeValue();
    QString version = documentElement.firstChildElement("ipxact:version").firstChild().nodeValue();

    return VLNV(type, vendor, library, name, version);
}
