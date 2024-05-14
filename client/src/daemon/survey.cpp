#include "survey.hpp"

Query::Query(const QString& id, const QString& dataKey,
    const QList<QString>& cohorts, const bool& discrete)
    : id(id)
    , dataKey(dataKey)
    , cohorts(cohorts)
    , discrete(discrete)
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

        const auto commissionerJson = object["commissioner"].toObject();
        survey->commissioner = QSharedPointer<Commissioner>::create(
            commissionerJson["name"].toString());

        for (const auto& item : object["queries"].toArray()) {
            const auto object = item.toObject();
            const auto id = object["id"].toString();
            const auto dataKey = object["data_key"].toString();

            const auto cohortsJson = object["cohorts"].toArray();
            QList<QString> cohorts;
            for (const auto& item : cohortsJson)
                cohorts.append(item.toString());

            const auto discrete = object["discrete"].toBool();

            survey->queries.push_back(
                QSharedPointer<Query>::create(id, dataKey, cohorts, discrete));
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
