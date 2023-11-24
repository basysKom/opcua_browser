#include <QMetaEnum>

#include "attribute.h"

static QString nodeAttributeToString(QOpcUa::NodeAttribute attribute)
{
    const QMetaEnum metaEnum = QMetaEnum::fromType<QOpcUa::NodeAttribute>();
    return metaEnum.valueToKey(int(attribute));
}

Attribute::Attribute(QOpcUa::NodeAttribute attribute, const QString &value)
    : mAttribute(attribute), mAttributeName(nodeAttributeToString(attribute)), mValue(value)
{
}

QOpcUa::NodeAttribute Attribute::attribute() const noexcept
{
    return mAttribute;
}

const QString &Attribute::attributeName() const noexcept
{
    return mAttributeName;
}

const QString &Attribute::value() const noexcept
{
    return mValue;
}

void Attribute::setValue(const QString &value)
{
    mValue = value;
}
