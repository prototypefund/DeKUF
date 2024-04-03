#include "commissioner.hpp"

#include "survey.hpp"

Query::Query(const QString& dataKey)
    : dataKey(dataKey)
{
}

QList<QSharedPointer<Survey>> Survey::listFromByteArray(const QByteArray& data)
{
    QList<QSharedPointer<Survey>> surveys;
    auto document = QJsonDocument::fromJson(data);
    for (auto item : document.array()) {
        auto object = item.toObject();
        auto id = object["id"].toString();
        auto name = object["name"].toString();
        QSharedPointer<Survey> survey(new Survey(id, name));

        for (auto item : object["commissioners"].toArray()) {
            auto object = item.toObject();
            auto name = object["name"].toString();
            survey->commissioners.push_back(
                QSharedPointer<Commissioner>::create(name));
        }

        for (auto item : object["queries"].toArray()) {
            auto object = item.toObject();
            auto dataKey = object["dataKey"].toString();
            survey->queries.push_back(QSharedPointer<Query>::create(dataKey));
        }
        surveys.push_back(survey);
    }
    return surveys;
}

Survey::Survey(const QString& id, const QString& name)
    : id(id)
    , name(name)
{
}
