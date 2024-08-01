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

Result<QSharedPointer<Survey>> Survey::fromByteArray(const QByteArray& data)
{
    try {
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
            survey->queries.push_back(QSharedPointer<Query>::create(
                queryId, dataKey, cohorts, discrete));
        }
        return Result(survey);
    } catch (const QJsonParseError& error) {
        return Result<QSharedPointer<Survey>>::Failure(error.errorString());
    }
}

Result<QList<QSharedPointer<Survey>>> Survey::listFromByteArray(
    const QByteArray& data)
{
    QList<QSharedPointer<Survey>> surveys;
    const auto document = QJsonDocument::fromJson(data);
    for (const auto& item : document.array()) {
        const auto byteArray
            = QJsonDocument(item.toObject()).toJson(QJsonDocument::Compact);
        auto parsingResult = fromByteArray(byteArray);
        if (!parsingResult.isSuccess())
            return Result<QList<QSharedPointer<Survey>>>::Failure(
                parsingResult.getErrorMessage());
        surveys.push_back(parsingResult.getValue());
    }
    return Result(surveys);
}

// TODO: Rename to toJsonByteArray.
QByteArray Survey::toByteArray() const
{
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
