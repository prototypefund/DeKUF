from rest_framework import serializers

from .models import Commissioner, Query, QueryResponse, Survey, SurveyResponse


class CommissionerSerializer(serializers.ModelSerializer):
    class Meta:
        model = Commissioner
        fields = ("name",)


class QuerySerializer(serializers.ModelSerializer):
    dataKey = serializers.CharField(source="data_key")
    discrete = serializers.ReadOnlyField()

    class Meta:
        model = Query
        fields = ["id", "dataKey", "cohorts", "discrete"]


class SurveySerializer(serializers.ModelSerializer):
    commissioners = CommissionerSerializer(many=True, read_only=True)
    queries = QuerySerializer(many=True)

    class Meta:
        model = Survey
        fields = ["id", "name", "commissioners", "queries"]


class QueryResponseSerializer(serializers.ModelSerializer):
    dataKey = serializers.CharField(source="data_key")

    class Meta:
        model = QueryResponse
        fields = ("dataKey", "data")


class SurveyResponseSerializer(serializers.ModelSerializer):
    commissioners = CommissionerSerializer(many=True)
    queryResponses = QueryResponseSerializer(many=True)

    class Meta:
        model = SurveyResponse
        fields = ("commissioners", "queryResponses")

    def create(self, validated_data):
        commissioner_data = validated_data.pop("commissioners")
        query_responses_data = validated_data.pop("queryResponses")
        survey_response = SurveyResponse.objects.create()

        for commissioner in commissioner_data:
            commission, created = Commissioner.objects.get_or_create(
                **commissioner
            )
            survey_response.commissioners.add(commission)

        for query_response in query_responses_data:
            QueryResponse.objects.create(
                survey_response=survey_response, **query_response
            )

        return survey_response
