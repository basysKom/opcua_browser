#ifndef REFERENCEMODEL_H
#define REFERENCEMODEL_H

#include <QAbstractListModel>

#include "reference.h"

class ReferenceModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ReferenceModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void addReference(const QString &type, bool isForward, const QString &target);
    void setReferences(const QList<Reference> &references);
    void clearReferences();
    void clearForwardReferences();

private:
    QList<Reference> mReferences;
};

#endif // REFERENCEMODEL_H
