#include <QMetaEnum>

#include "attribute.h"

static QString nodeAttributeToString(QOpcUa::NodeAttribute attribute)
{
    const QMetaEnum metaEnum = QMetaEnum::fromType<QOpcUa::NodeAttribute>();
    return metaEnum.valueToKey(int(attribute));
}

Attribute::Attribute(QOpcUa::NodeAttribute attribute, const QString &value)
    : mAttribute(nodeAttributeToString(attribute))
    , mValue(value)
{

}

const QString &Attribute::attribute() const noexcept
{
    return mAttribute;
}

const QString &Attribute::value() const noexcept
{
    return mValue;
}
