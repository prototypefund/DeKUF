#include "survey.hpp"

Query::Query(const QString& dataKey)
    : dataKey(dataKey)
{
}

QList<QSharedPointer<Survey>> Survey::listFromByteArray(const QByteArray& data)
{
    QList<QSharedPointer<Survey>> surveys;
    const auto document = QJsonDocument::fromJson(data);
    for (const auto& item : document.array()) {
        const auto object = item.toObject();
        const auto id = object["id"].toString();
        const auto name = object["name"].toString();
        auto survey = QSharedPointer<Survey>::create(id, name);

        for (const auto& item : object["commissioners"].toArray()) {
            const auto object = item.toObject();
            const auto name = object["name"].toString();
            survey->commissioners.push_back(
                QSharedPointer<Commissioner>::create(name));
        }

        for (const auto& item : object["queries"].toArray()) {
            const auto object = item.toObject();
            const auto dataKey = object["dataKey"].toString();
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
