/* 
 *
 *  Created on: 27.7.2010
 *      Author: Antti Kamppi
 */

#include "generaldeclarations.h"
//#include "PortMap.h"
#include <IPXACTmodels/common/Vector.h>
#include "XmlUtils.h"

#include <QDomNode>
#include <QString>
#include <QDomNodeList>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QObject>
#include <QMap>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <qmath.h>

#include <QDebug>

General::PortBounds::PortBounds():
portName_(),
left_(0),
right_(0) {
}

General::PortBounds::PortBounds( const QString& portName ):
portName_(portName),
left_(0),
right_(0) {
}

General::PortBounds::PortBounds( const QString& portName, const int left, const int right ):
portName_(portName),
left_(left),
right_(right) {
}

General::PortBounds::PortBounds( const PortBounds& other ):
portName_(other.portName_),
left_(other.left_),
right_(other.right_) {
}

General::PortBounds& General::PortBounds::operator=( const PortBounds& other ) {
	if (&other != this) {
		portName_ = other.portName_;
		left_ = other.left_;
		right_ = other.right_;
	}
	return *this;
}

bool General::PortBounds::operator<( const PortBounds& other ) const {
	return portName_ < other.portName_;
}

bool General::PortBounds::operator==( const PortBounds& other ) const {
	return portName_ == other.portName_;
}

bool General::PortBounds::operator!=( const PortBounds& other ) const {
	return portName_ != other.portName_;
}

General::PortAlignment::PortAlignment():
port1Left_(-1),
port1Right_(-1),
port2Left_(-1),
port2Right_(-1),
invalidAlignment_(true) {
}

General::PortAlignment::PortAlignment( const PortAlignment& other ):
port1Left_(other.port1Left_),
port1Right_(other.port1Right_),
port2Left_(other.port2Left_),
port2Right_(other.port2Right_),
invalidAlignment_(other.invalidAlignment_) {
}

General::PortAlignment& General::PortAlignment::operator=( const PortAlignment& other ) {
	if (this != &other) {
		port1Left_ = other.port1Left_;
		port1Right_ = other.port1Right_;
		port2Left_ = other.port2Left_;
		port2Right_ = other.port2Right_;
		invalidAlignment_ = other.invalidAlignment_;
	}
	return *this;
}
/*
/*General::PortAlignment General::calculatePortAlignment( 
	const PortMap* portMap1, 
	int phys1LeftBound, 
	int phys1RightBound, 
	const PortMap* portMap2,
	int phys2LeftBound, 
	int phys2RightBound ) {

	General::PortAlignment alignment;
	
	// if the port maps are for different logical ports
	if (portMap1->logicalPort() != portMap2->logicalPort()) {
		return alignment;
	}

	// if port 1 is vectored on the port map
	if (portMap1->physicalVector()) {
		phys1LeftBound = portMap1->getPhysicalLeft();
		phys1RightBound = portMap1->physicalVector()->getRight().toInt();
	}

	// if port 2 is vectored on the port map
	if (portMap2->physicalVector()) {
		phys2LeftBound = portMap2->getPhysicalLeft();
		phys2RightBound = portMap2->physicalVector()->getRight().toInt();
	}

	// if both have vectored logical signals
	if (portMap1->logicalVector() && portMap2->logicalVector()) {

		// if the vectored ports don't have any common bounds
		if (portMap1->getLogicalRight() > portMap2->getLogicalLeft() ||
			portMap1->getLogicalLeft() < portMap2->getLogicalRight()) {
				return alignment;
		}

		int logicalLeft = qMin(portMap1->getLogicalLeft(), 
			portMap2->getLogicalLeft());
		int logicalRight = qMax(portMap1->getLogicalRight(),
			portMap2->getLogicalRight());

		{
			// count how much the left bound of port 1 has to be adjusted down
			int downSize = abs(portMap1->getLogicalLeft() - logicalLeft);
			// count how must the right bound of  port 1 has to be adjusted up
			int upSize = abs(logicalRight - portMap1->getLogicalRight());

			alignment.port1Left_ = phys1LeftBound - downSize;
			alignment.port1Right_ = phys1RightBound + upSize;
		}
		{
			// count how much the left bound of port 2 has to be adjusted down
			int downSize = abs(portMap2->getLogicalLeft() - logicalLeft);
			// count how must the right bound of  port 2 has to be adjusted up
			int upSize = abs(logicalRight - portMap2->getLogicalRight());

			alignment.port2Left_ = phys2LeftBound - downSize;
			alignment.port2Right_ = phys2RightBound + upSize;
		}
	}
	// if port map1 has vectored logical signal
	else if (portMap1->logicalVector() && !portMap2->logicalVector()) {

		// port 1 uses the original physical bounds
		alignment.port1Left_ = phys1LeftBound;
		alignment.port1Right_ = phys1RightBound;

		// port 2 uses the bounds of the logical port of port 1
		alignment.port2Left_ = portMap1->getLogicalLeft();
		alignment.port2Right_ = portMap1->getLogicalRight();
	}
	// if port map2 has vectored logical signal
	else if (!portMap1->logicalVector() && portMap2->logicalVector()) {

		// port 1 uses the bounds of the logical port of port 2
		alignment.port1Left_ = portMap2->getLogicalLeft();
		alignment.port1Right_ = portMap2->getLogicalRight();

		// port 2 uses the original physical bounds
		alignment.port2Left_ = phys2LeftBound;
		alignment.port2Right_ = phys2RightBound;
	}
	// if neither has vectored logical signal
	else {

		// both ports use the original physical bounds
		alignment.port1Left_ = phys1LeftBound;
		alignment.port1Right_ = phys1RightBound;
		alignment.port2Left_ = phys2LeftBound;
		alignment.port2Right_ = phys2RightBound;
	}

	// check if the sizes of the ports match
	int port1Size = alignment.port1Left_ - alignment.port1Right_ + 1;
	int port2Size = alignment.port2Left_ - alignment.port2Right_ + 1;
	if (port1Size != port2Size) {
		alignment.invalidAlignment_ = true;
	}
	else {
		alignment.invalidAlignment_ = false;
	}
	return alignment;
}*/

QString General::port2String(const QString& portName, int leftBound, int rightBound) {
	QString str(portName);
	str += QString("[%1..%2]").arg(leftBound).arg(rightBound);
	return str;
}

QString General::bool2Str(bool value) {
	if (value) {
		return QString("true");
	}
	else {
		return QString("false");
	}
}

General::Usage General::str2Usage(QString str,
		General::Usage defaultValue) {
	if (str == QString("memory")) {
		return General::MEMORY;
	}
	else if (str == QString("register")) {
		return General::REGISTER;
	}
	else if (str ==	QString("reserved")) {
		return General::RESERVED;
	}
	else {
		return defaultValue;
	}
}

QString General::usage2Str(const General::Usage usage) {
	switch (usage) {
	case General::MEMORY: {
		return QString("memory");
	}
	case General::REGISTER: {
		return QString("register");
	}
	case General::RESERVED: {
		return QString("reserved");
	}
	// if UNSPECIFIED_USAGE
	default: {
		return QString();
	}
	}
}

General::DriverType General::str2DriverType(QString str, General::DriverType defaultValue)
{
	if (str == "any")
    {
		return General::ANY;
	}
	else if (str == "clock")
    {
		return General::CLOCK;
	}
	else if (str == "singleShot")
    {
		return General::SINGLESHOT;
	}
    else if (str == "none")
    {
        return General::NO_DRIVER;
    }
	else
    {
		return defaultValue;
	}
}

QString General::driverType2Str(General::DriverType type)
{
    if (type == General::ANY)
    {
        return QString("any");
    }
    else if (type == General::CLOCK)
    {
        return QString("clock");
    }
    else if (type == General::SINGLESHOT)
    {
        return QString("singleShot");
    }
    else 
    {
        return QString();
    }
}

bool General::str2Bool(const QString str, bool defaultValue) {
	if (str == QString("true")) {
		return true;
	}
	else if (str == QString("false")) {
		return false;
	}
	else {
		return defaultValue;
	}
}


General::InterfaceMode General::str2Interfacemode(const QString& str, InterfaceMode defaultValue) {
	
	// check all known interface mode names
	for (unsigned int i = 0; i < General::INTERFACE_MODE_COUNT; ++i) {
		
		// if match is found
		if (str.compare(General::INTERFACE_MODE_NAMES[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::InterfaceMode>(i);
		}
	}

	// if there was no match
	return defaultValue;
}

QString General::interfaceMode2Str(const General::InterfaceMode mode) {
	return General::INTERFACE_MODE_NAMES[mode];
}

//-----------------------------------------------------------------------------
// Function: getCompatibleInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode General::getCompatibleInterfaceMode(InterfaceMode mode)
{
    switch (mode)
    {
    case General::MASTER: {
            return MIRROREDMASTER;
        }
    case General::SLAVE: {
            return MIRROREDSLAVE;
        }
    case General::SYSTEM: {
            return MIRROREDSYSTEM;
        }
    case General::MIRROREDMASTER: {
            return MASTER;
        }
    case General::MIRROREDSLAVE: {
            return SLAVE;
        }
    case General::MIRROREDSYSTEM: {
            return SYSTEM;
        }
    case General::MONITOR: {
            return MONITOR;
        }
    default: {
            Q_ASSERT(false);
            return MASTER;
        }
    }
}

General::LibraryFilePair::LibraryFilePair(const QString filePath,
		const QString libraryName): filePath_(filePath),
		libraryName_(libraryName) {
}

General::LibraryFilePair::LibraryFilePair( const LibraryFilePair& other ):
filePath_(other.filePath_),
libraryName_(other.libraryName_) {
}

bool General::LibraryFilePair::operator==( const LibraryFilePair& other ) const {
	return other.filePath_ == this->filePath_ && 
		other.libraryName_ == this->libraryName_;
}

General::LibraryFilePair& General::LibraryFilePair::operator=( const LibraryFilePair& other ) {
	if (this != &other) {
		filePath_ = other.filePath_;
		libraryName_ = other.libraryName_;
	}
	return *this;
}

QString General::getRelativePath(const QString from, const QString to) {
	if (from.isEmpty() || to.isEmpty()) {
		return QString();
	}

	// create file info instance to make sure that only the directory of the
	// from parameter is used
	QFileInfo fromInfo(from);

    QString fromPath = fromInfo.absolutePath();

    if (fromInfo.isDir())
    {
        fromPath = fromInfo.absoluteFilePath();
    }

	// if the directory does not exist
	QDir ipXactDir(fromPath);

	if (!ipXactDir.exists()) {
		return QString();
	}

	// create file info instance to make sure the target file exists and to
	// use an absolute file path to calculate the relative path.
	QFileInfo toInfo(to);
	if (!toInfo.exists()) {
		return QString();
	}

	// calculate the relative path and return it.
	QString relPath = ipXactDir.relativeFilePath(toInfo.absoluteFilePath());

    // Strip the ending slash if found.
    if (relPath.size() > 0 && relPath.at(relPath.size() - 1) == '/')
    {
        relPath = relPath.left(relPath.size() - 1);
    }
    else if (relPath.isEmpty())
    {
        relPath = ".";
    }

    return relPath;
}

QString General::getRelativeSavePath( const QString& from, const QString& to ) {
	if (from.isEmpty() || to.isEmpty()) {
		return QString();
	}

	// create file info instance to make sure that only the directory of the
	// from parameter is used
	QFileInfo fromInfo(from);

	QString fromPath = fromInfo.absolutePath();

	if (fromInfo.isDir())
	{
		fromPath = fromInfo.absoluteFilePath();
	}

	// if the directory does not exist
	QDir ipXactDir(fromPath);

	if (!ipXactDir.exists()) {
		return QString();
	}

	// create file info instance to make sure the target file exists and to
	// use an absolute file path to calculate the relative path.
	QFileInfo toInfo(to);

	// calculate the relative path and return it.
	return ipXactDir.relativeFilePath(toInfo.absoluteFilePath());
}

QString General::getAbsolutePath(const QString& originalPath,
		const QString& relativePath) {

	// if one of the parameters is empty
	if (originalPath.isEmpty() || relativePath.isEmpty()) {
		return QString();
	}

	QFileInfo relativeInfo(relativePath);
	if (relativeInfo.isAbsolute()) {
		return relativePath;
	}

	// get the directory path of the original path
	QFileInfo original(originalPath);
	QDir originalDir(original.absolutePath());

	// make sure the path exists
	return originalDir.absoluteFilePath(relativePath);
}

General::ModifiedWrite General::str2ModifiedWrite( const QString& str ) {
	
	// check all defined strings
	for (unsigned int i = 0; i < General::MODIFIED_WRITE_COUNT; ++i) {
		
		// if a match is found
		if (str.compare(General::MODIFIED_WRITE_STRINGS[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::ModifiedWrite>(i);
		}
	}

	// if none of the defined strings matched 
	return General::MODIFIED_WRITE_COUNT;
}

QString General::modifiedWrite2Str( const General::ModifiedWrite modWrite ) {
	return General::MODIFIED_WRITE_STRINGS[modWrite];
}

QString General::readAction2Str( const General::ReadAction readAction ) {
	return General::READ_ACTION_STRINGS[readAction];
}

General::ReadAction General::str2ReadAction( const QString& str ) {
	// check all defined strings
	for (unsigned int i = 0; i < General::READ_ACTION_COUNT; ++i) {
		
		// if a match is found
		if (str.compare(General::READ_ACTION_STRINGS[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::ReadAction>(i);
		}
	}

	// if none of the defined strings matched
	return General::READ_ACTION_COUNT;
}

QString General::testConstraint2Str( const General::TestConstraint testConstraint ) {
	return General::TEST_CONSTRAINT_STRINGS[testConstraint];
}

General::TestConstraint General::str2TestConstraint( const QString& str ) {
	// check all defined strings
	for (unsigned int i = 0; i < General::TESTCONSTRAINT_COUNT; ++i) {

		// if a match is found
		if (str.compare(General::TEST_CONSTRAINT_STRINGS[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::TestConstraint>(i);
		}
	}

	// if none of the defined strings matched
	return General::TESTCONSTRAINT_COUNT;
}



quint64 General::str2Uint( const QString& str ) {

    if (str.isEmpty()) {
        return 0;
    }

    // used to detect if the conversion was successful
    bool success = true;
    quint64 number = 0;

    // if starts with "0x" then it is hexadecimal digit
    if (str.startsWith("0x", Qt::CaseInsensitive)) {

        number = str.toULongLong(&success, 16);
        if (success) {
            return number;
        }
    }

    // needed because the last letter is chopped if one is found
    QString strNumber = str;

    // the multiple is the last letter if one exists
    const QChar multiple = strNumber.at(strNumber.size()-1);
    quint64 multiplier = 1;

    // get the correct multiplier and remove the letter from the string
    if (multiple == 'k' || multiple == 'K') {
        multiplier = qPow(2, 10);
        strNumber.chop(1);
    }
    else if (multiple == 'M') {
        multiplier = qPow(2, 20);
        strNumber.chop(1);
    }
    else if (multiple == 'G') {
        multiplier = qPow(2, 30);
        strNumber.chop(1);
    }
    else if (multiple == 'T') {
        multiplier = qPow(2, 40);
        strNumber.chop(1);
    }
    else if (multiple == 'P') {
        multiplier = qPow(2, 50);
        strNumber.chop(1);
    }

    // try to convert the number 
    number = strNumber.toULongLong(&success);

    // if the conversion failed
    if (!success) {
        return 0;
    }
    // otherwise return the correct int-format
    else {
        return number * multiplier;
    }
}