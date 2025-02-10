/* Port interface file for Python */
%module Port

%include "QString.i"
%include "NameGroup.i"

%{
#include "IPXACTmodels\common\Port.h"
%}

class Port : public NameGroup
{
public:

	QString getValue() const;
//	void setValue(QString const& value);
    
    QString getType() const;
//    void setType(QString const& type);

};
