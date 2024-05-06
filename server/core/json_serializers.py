from rest_framework import serializers

from .models import Commissioner, Query, QueryResponse, Survey, SurveyResponse


class CommissionerSerializer(serializers.ModelSerializer):
    class Meta:
        model = Commissioner
        fields = ("name",)


class QuerySerializer(serializers.ModelSerializer):
    dataKey = serializers.CharField(source="data_key")
    discrete = serializers.BooleanField(source="discrete")

    class Meta:
        model = Query
        fields = ["id", "dataKey", "cohorts", "discrete"]


class SurveySerializer(serializers.ModelSerializer):
    commissioner = CommissionerSerializer(many=True, read_only=True)
    queries = QuerySerializer(many=True)

    class Meta:
        model = Survey
        fields = ["id", "name", "commissioner", "queries"]


class QueryResponseSerializer(serializers.ModelSerializer):
    dataKey = serializers.CharField(source="data_key")

    class Meta:
        model = QueryResponse
        fields = ("dataKey", "data")


class SurveyResponseSerializer(serializers.ModelSerializer):
    queryResponses = QueryResponseSerializer(many=True)
    surveyId = serializers.PrimaryKeyRelatedField(
        queryset=Survey.objects.all(), source="survey"
    )

    class Meta:
        model = SurveyResponse
        fields = ("surveyId", "queryResponses")

    def create(self, validated_data):
        query_responses_data = validated_data.pop('queryResponses')
        survey_response = SurveyResponse.objects.create(**validated_data)

        for query_response_data in query_responses_data:
            survey_response.query_responses.create(**query_response_data)

        return survey_response
