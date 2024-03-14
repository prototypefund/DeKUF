#pragma once

#include <QtCore>

class Query {
public:
    const QString dataKey;

    Query(const QString& dataKey)
        : dataKey(dataKey)
    {
    }
};

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
            for (auto item : object["queries"].toArray()) {
                auto object = item.toObject();
                auto dataKey = object["dataKey"].toString();
                QSharedPointer<Query> query(new Query(dataKey));
                survey->queries.push_back(query);
            }
            surveys.push_back(survey);
        }
        return surveys;
    }

    const QString id;
    const QString name;
    QList<QSharedPointer<Query>> queries;

    Survey(const QString& id, const QString& name)
        : id(id)
        , name(name)
    {
    }
};
