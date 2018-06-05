/* 
 *
 *  Created on: 27.7.2010
 *      Author: Antti Kamppi
 */

#include "generaldeclarations.h"

#include "XmlUtils.h"

#include <QString>
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

QString General::port2String(const QString& portName, int leftBound, int rightBound) {
	QString str(portName);
	str.append(QStringLiteral("[%1..%2]").arg(leftBound).arg(rightBound));
	return str;
}

QString General::bool2Str(bool value) {
	if (value) {
		return QStringLiteral("true");
	}
	else {
		return QStringLiteral("false");
	}
}

General::Usage General::str2Usage(QString str,
		General::Usage defaultValue) {
	if (str == QLatin1String("memory")) {
		return General::MEMORY;
	}
	else if (str == QLatin1String("register")) {
		return General::REGISTER;
	}
	else if (str ==	QLatin1String("reserved")) {
		return General::RESERVED;
	}
	else {
		return defaultValue;
	}
}

QString General::usage2Str(const General::Usage usage) {
	switch (usage) {
	case General::MEMORY: {
		return QStringLiteral("memory");
	}
	case General::REGISTER: {
		return QStringLiteral("register");
	}
	case General::RESERVED: {
		return QStringLiteral("reserved");
	}
	// if UNSPECIFIED_USAGE
	default: {
		return QString();
	}
	}
}

General::DriverType General::str2DriverType(QString str, General::DriverType defaultValue)
{
	if (str == QLatin1String("any"))
    {
		return General::ANY;
	}
	else if (str == QLatin1String("clock"))
    {
		return General::CLOCK;
	}
	else if (str == QLatin1String("singleShot"))
    {
		return General::SINGLESHOT;
	}
    else if (str == QLatin1String("none"))
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
        return QStringLiteral("any");
    }
    else if (type == General::CLOCK)
    {
        return QStringLiteral("clock");
    }
    else if (type == General::SINGLESHOT)
    {
        return QStringLiteral("singleShot");
    }
    else 
    {
        return QString();
    }
}

bool General::str2Bool(const QString str, bool defaultValue) {
	if (str == QLatin1String("true")) {
		return true;
	}
	else if (str == QLatin1String("false")) {
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
            return INTERFACE_MODE_COUNT;
        }
    }
}

QString General::getRelativePath(QString from, QString to)
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
    if (relPath.size() > 0 && relPath.at(relPath.size() - 1) == QLatin1Char('/'))
    {
        relPath = relPath.left(relPath.size() - 1);
    }
    else if (relPath.isEmpty())
    {
        relPath = QStringLiteral(".");
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



quint64 General::str2Uint( const QString& str )
{
    if (str.isEmpty())
    {
        return 0;
    }

    // used to detect if the conversion was successful
    bool success = true;
    quint64 number = 0;

    // if starts with "0x" then it is hexadecimal digit
    if (str.startsWith(QStringLiteral("0x"), Qt::CaseInsensitive))
    {
        number = str.toULongLong(&success, 16);
        if (success)
        {
            return number;
        }
    }

    // needed because the last letter is chopped if one is found
    QString strNumber = str;

    // the multiple is the last letter if one exists
    const QChar multiple = strNumber.at(strNumber.size()-1);
    quint64 multiplier = 1;

    // get the correct multiplier and remove the letter from the string
    if (multiple == QLatin1Char('k') || multiple == QLatin1Char('K'))
    {
        multiplier = qPow(2, 10);
        strNumber.chop(1);
    }
    else if (multiple == QLatin1Char('M'))
    {
        multiplier = qPow(2, 20);
        strNumber.chop(1);
    }
    else if (multiple == QLatin1Char('G'))
    {
        multiplier = qPow(2, 30);
        strNumber.chop(1);
    }
    else if (multiple == QLatin1Char('T'))
    {
        multiplier = qPow(2, 40);
        strNumber.chop(1);
    }
    else if (multiple == QLatin1Char('P'))
    {
        multiplier = qPow(2, 50);
        strNumber.chop(1);
    }

    // try to convert the number 
    number = strNumber.toULongLong(&success);

    // if the conversion failed
    if (!success)
    {
        return 0;
    }
    // otherwise return the correct int-format
    else
    {
        return number * multiplier;
    }
}