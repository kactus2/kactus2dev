/* NameGroup interface file for Python */
%module NameGroup

%include "QString.i"

%{
#include "IPXACTmodels\common\NameGroup.h"
%}

class NameGroup
{

public:

	NameGroup();
	NameGroup(QString const& name, QString const& displayName = QString(), QString const& description = QString());
	NameGroup(const NameGroup& other);

    virtual ~NameGroup();

    QString name() const;
//    void setName(QString const& name);

    QString displayName() const;
//    void setDisplayName(QString const& displayName);

    QString description() const;
//    void setDescription(QString const& description);
};
