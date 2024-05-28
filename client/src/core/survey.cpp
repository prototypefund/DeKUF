#include "survey.hpp"

Query::Query(const QString& id, const QString& dataKey,
    const QList<QString>& cohorts, const bool& discrete)
    : id(id)
    , dataKey(dataKey)
    , cohorts(cohorts)
    , discrete(discrete)
{
}

Survey::Survey(const QString& id, const QString& name)
    : id(id)
    , name(name)
{
}

QSharedPointer<Survey> Survey::fromByteArray(const QByteArray& data)
{
    QSharedPointer<Survey> survey;
    const auto object = QJsonDocument::fromJson(data).object();
    const auto id = object["id"].toString();
    const auto name = object["name"].toString();
    survey = QSharedPointer<Survey>::create(id, name);

    const auto commissionerJson = object["commissioner"].toObject();
    survey->commissioner = QSharedPointer<Commissioner>::create(
        commissionerJson["name"].toString());

    for (const auto& item : object["queries"].toArray()) {
        const auto queryObject = item.toObject();
        const auto queryId = queryObject["id"].toString();
        const auto dataKey = queryObject["data_key"].toString();

        QList<QString> cohorts;
        for (const auto& cohortItem : queryObject["cohorts"].toArray()) {
            cohorts.append(cohortItem.toString());
        }

        const auto discrete = queryObject["discrete"].toBool();
        survey->queries.push_back(
            QSharedPointer<Query>::create(queryId, dataKey, cohorts, discrete));
    }
    return survey;
}

QList<QSharedPointer<Survey>> Survey::listFromByteArray(const QByteArray& data)
{
    QList<QSharedPointer<Survey>> surveys;
    const auto document = QJsonDocument::fromJson(data);
    for (const auto& item : document.array()) {
        const auto byteArray
            = QJsonDocument(item.toObject()).toJson(QJsonDocument::Compact);
        surveys.push_back(fromByteArray(byteArray));
    }
    return surveys;
}

QByteArray Survey::toByteArray() {
    QJsonObject object;
    object["id"] = id;
    object["name"] = name;

    if (commissioner) {
        QJsonObject commissionerJson;
        commissionerJson["name"] = commissioner->name;
        object["commissioner"] = commissionerJson;
    }

    QJsonArray queriesArray;
    for (const auto& query : queries) {
        QJsonObject queryObject;
        queryObject["id"] = query->id;
        queryObject["data_key"] = query->dataKey;

        QJsonArray cohortsArray;
        for (const auto& cohort : query->cohorts) {
            cohortsArray.append(QJsonValue(cohort));
        }

        queryObject["cohorts"] = cohortsArray;
        queryObject["discrete"] = query->discrete;
        queriesArray.append(queryObject);
    }

    object["queries"] = queriesArray;

    const QJsonDocument doc(object);
    return doc.toJson(QJsonDocument::Compact);
}
