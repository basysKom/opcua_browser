#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <QString>
#include <qopcuatype.h>

class Attribute
{
public:
    Attribute(QOpcUa::NodeAttribute attribute, const QString &value);

    const QString &attribute() const noexcept;
    const QString &value() const noexcept;

private:
    QString mAttribute;
    QString mValue;
};

#endif // ATTRIBUTE_H
