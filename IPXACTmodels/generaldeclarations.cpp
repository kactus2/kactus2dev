/* 
 *
 *  Created on: 27.7.2010
 *      Author: Antti Kamppi
 */

#include "generaldeclarations.h"

#include <IPXACTmodels/utilities/XmlUtils.h>

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


General::PortBounds::PortBounds(): portName_(), left_(0), right_(0)
{

}

General::PortBounds::PortBounds( const QString& portName ): portName_(portName), left_(0), right_(0)
{

}

General::PortBounds::PortBounds( const QString& portName, const int left, const int right ):
portName_(portName),
left_(left),
right_(right)
{

}

bool General::PortBounds::operator<( const PortBounds& other ) const
{
	return portName_ < other.portName_;
}

bool General::PortBounds::operator==( const PortBounds& other ) const
{
	return portName_ == other.portName_;
}

bool General::PortBounds::operator!=( const PortBounds& other ) const
{
	return portName_ != other.portName_;
}

General::PortAlignment::PortAlignment():
port1Left_(-1),
port1Right_(-1),
port2Left_(-1),
port2Right_(-1),
invalidAlignment_(true)
{

}

QString General::port2String(const QString& portName, int leftBound, int rightBound)
{
	QString str(portName);
	str.append(QStringLiteral("[%1..%2]").arg(leftBound).arg(rightBound));
	return str;
}

QString General::bool2Str(bool value)
{
	if (value)
    {
		return QStringLiteral("true");
	}
	else
    {
		return QStringLiteral("false");
	}
}

General::Usage General::str2Usage(QString str, General::Usage defaultValue)
{
	if (str == QLatin1String("memory"))
    {
		return General::MEMORY;
	}
	else if (str == QLatin1String("register"))
    {
		return General::REGISTER;
	}
	else if (str ==	QLatin1String("reserved"))
    {
		return General::RESERVED;
	}
	else
    {
		return defaultValue;
	}
}

QString General::usage2Str(const General::Usage usage)
{
	if (usage == General::MEMORY)
    {
		return QStringLiteral("memory");
	}
    else if (usage == General::REGISTER)
    {
		return QStringLiteral("register");
	}
	else if (usage == General::RESERVED)
    {
		return QStringLiteral("reserved");
	}
    else
    {
        return QString();	
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

bool General::str2Bool(const QString str, bool defaultValue)
{
	if (str == QLatin1String("true"))
    {
		return true;
	}
	else if (str == QLatin1String("false"))
    {
		return false;
	}
	else
    {
		return defaultValue;
	}
}


General::InterfaceMode General::str2Interfacemode(const QString& str, InterfaceMode defaultValue)
{
	// check all known interface mode names
	for (unsigned int i = 0; i < General::INTERFACE_MODE_COUNT; ++i)
    {
		if (str.compare(General::INTERFACE_MODE_NAMES[i], Qt::CaseInsensitive) == 0)
        {
			return static_cast<General::InterfaceMode>(i);
		}
	}

	// if there was no match
	return defaultValue;
}

QString General::interfaceMode2Str(const General::InterfaceMode mode)
{
 	return General::INTERFACE_MODE_NAMES[mode];
}

//-----------------------------------------------------------------------------
// Function: General::getCompatibleInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode General::getCompatibleInterfaceMode(InterfaceMode mode)
{
    if (mode == General::MASTER)
    {
        return MIRROREDMASTER;
    }
    else if (mode == General::SLAVE)
    {
        return MIRROREDSLAVE;
    }
    else if (mode == General::SYSTEM)
    {
        return MIRROREDSYSTEM;
    }
    else if (mode == General::MIRROREDMASTER)
    {
        return MASTER;
    }
    else if (mode == General::MIRROREDSLAVE)
    {
        return SLAVE;
    }
    else if (mode == General::MIRROREDSYSTEM)
    {
        return SYSTEM;
    }
    else if (mode == General::MONITOR)
    {
        return MONITOR;
    }
    else
    {
        return INTERFACE_MODE_COUNT;
    }
}

//-----------------------------------------------------------------------------
// Function: generaldeclarations::getCompatibleInterfaceModeList()
//-----------------------------------------------------------------------------
QVector<General::InterfaceMode> General::getCompatibleInterfaceModesForActiveInterface(General::InterfaceMode mode)
{
    QVector<General::InterfaceMode> compatibleModes;

    if (mode == General::MASTER)
    {
        //! Master -> slave / mirrored master
        compatibleModes = { General::SLAVE, General::MIRROREDMASTER, General::MONITOR };
    }
    else if (mode == General::MIRROREDMASTER)
    {
        //! Mirrored Master -> Master
        compatibleModes = { General::MASTER, General::MONITOR };
    }
    else if (mode == General::SLAVE)
    {
        //! Slave -> master / mirrored slave
        compatibleModes = { General::MASTER, General::MIRROREDSLAVE, General::MONITOR };
    }
    else if (mode == General::MIRROREDSLAVE)
    {
        //! Mirrored slave -> slave
        compatibleModes = { General::SLAVE, General::MONITOR };
    }
    else if (mode == General::SYSTEM)
    {
        //! System -> Mirrored system
        compatibleModes = { General::MIRROREDSYSTEM, General::MONITOR };
    }
    else if (mode == General::MIRROREDSYSTEM)
    {
        //! Mirrored System -> System
        compatibleModes = { General::SYSTEM, General::MONITOR };
    }
    else if (mode == General::MONITOR)
    {
        //! Monitor Mode -> Mode
        //!     if Mode == System || MirroredSystem
        //!         -> System groups match
        compatibleModes = { General::MASTER, General::MIRROREDMASTER, General::SLAVE, General::MIRROREDSLAVE,
            General::SYSTEM, General::MIRROREDSYSTEM };
    }

    return compatibleModes;
}

//-----------------------------------------------------------------------------
// Function: generaldeclarations::getCompatibleInterfaceModeForHierarchicalInterface()
//-----------------------------------------------------------------------------
QVector<General::InterfaceMode> General::getCompatibleInterfaceModeForHierarchicalInterface(
    General::InterfaceMode mode)
{
    //! Hierarchical Interface -> Same Mode

    QVector<General::InterfaceMode> compatibleModes;
    compatibleModes.append(mode);
    return compatibleModes;
}

QString General::getRelativePath(QString from, QString to)  
{
    if (from.isEmpty() || to.isEmpty())
    {
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

QString General::getRelativeSavePath( const QString& from, const QString& to )
{
	if (from.isEmpty() || to.isEmpty())
    {
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

QString General::getAbsolutePath(const QString& originalPath, const QString& relativePath)
{
	if (originalPath.isEmpty() || relativePath.isEmpty())
    {
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

General::ModifiedWrite General::str2ModifiedWrite( const QString& str )
{	
	// check all defined strings
	for (unsigned int i = 0; i < General::MODIFIED_WRITE_COUNT; ++i)
    {				
		if (str.compare(General::MODIFIED_WRITE_STRINGS[i], Qt::CaseInsensitive) == 0)
        {
			return static_cast<General::ModifiedWrite>(i);
		}
	}

	// if none of the defined strings matched 
	return General::MODIFIED_WRITE_COUNT;
}

QString General::modifiedWrite2Str( const General::ModifiedWrite modWrite )
{
	return General::MODIFIED_WRITE_STRINGS[modWrite];
}

QString General::readAction2Str( const General::ReadAction readAction )
{
	return General::READ_ACTION_STRINGS[readAction];
}

General::ReadAction General::str2ReadAction( const QString& str )
{
	// check all defined strings
	for (unsigned int i = 0; i < General::READ_ACTION_COUNT; ++i)
    {
		if (str.compare(General::READ_ACTION_STRINGS[i], Qt::CaseInsensitive) == 0)
        {
			return static_cast<General::ReadAction>(i);
		}
	}

	// if none of the defined strings matched
	return General::READ_ACTION_COUNT;
}

QString General::testConstraint2Str( const General::TestConstraint testConstraint )
{
	return General::TEST_CONSTRAINT_STRINGS[testConstraint];
}

General::TestConstraint General::str2TestConstraint( const QString& str )
{
	// check all defined strings
	for (unsigned int i = 0; i < General::TESTCONSTRAINT_COUNT; ++i)
    {
		if (str.compare(General::TEST_CONSTRAINT_STRINGS[i], Qt::CaseInsensitive) == 0)
        {
			return static_cast<General::TestConstraint>(i);
		}
	}

	// if none of the defined strings matched
	return General::TESTCONSTRAINT_COUNT;
}
