/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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

    void addReference(const QString &type, const QString &typeNodeId, bool isForward,
                      const QString &target, const QString &targetNodeId);
    void setReferences(const QList<Reference> &references);
    void clearReferences();
    void clearForwardReferences();

private:
    QList<Reference> mReferences;
};

#endif // REFERENCEMODEL_H
