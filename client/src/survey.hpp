#pragma once

#include <QtCore>

class Survey {
public:
    static QList<QSharedPointer<Survey>> listFromByteArray(
        const QByteArray& data)
    {
        QList<QSharedPointer<Survey>> surveys;
        auto document = QJsonDocument::fromJson(data);
        for (auto item : document.array()) {
            auto object = item.toObject();
            auto id = object["id"].toString();
            auto name = object["name"].toString();
            QSharedPointer<Survey> survey(new Survey(id, name));
            surveys.push_back(survey);
        }
        return surveys;
    }

    const QString id;
    const QString name;

    Survey(const QString& id, const QString& name)
        : id(id)
        , name(name)
    {
    }
};
