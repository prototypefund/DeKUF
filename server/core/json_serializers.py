from rest_framework import serializers

from .models.commissioner import Commissioner
from .models.response import QueryResponse, SurveyResponse
from .models.survey import Query, Survey


class CommissionerSerializer(serializers.ModelSerializer):
    class Meta:
        model = Commissioner
        fields = ("name",)


class QuerySerializer(serializers.ModelSerializer):
    data_key = serializers.CharField()
    discrete = serializers.BooleanField()

    class Meta:
        model = Query
        fields = ["id", "data_key", "cohorts", "discrete"]


class SurveySerializer(serializers.ModelSerializer):
    commissioner = CommissionerSerializer(read_only=True)
    queries = QuerySerializer(many=True)

    class Meta:
        model = Survey
        fields = ["id", "name", "commissioner", "queries"]


class QueryResponseSerializer(serializers.ModelSerializer):
    query_id = serializers.PrimaryKeyRelatedField(
        queryset=Query.objects.all(), source="query"
    )

    class Meta:
        model = QueryResponse
        fields = ("data", "query_id")


class SurveyResponseSerializer(serializers.ModelSerializer):
    query_responses = QueryResponseSerializer(many=True)
    survey_id = serializers.PrimaryKeyRelatedField(
        queryset=Survey.objects.all(), source="survey"
    )

    class Meta:
        model = SurveyResponse
        fields = ("survey_id", "query_responses")

    def create(self, validated_data):
        query_responses_data = validated_data.pop("query_responses")
        survey_response = SurveyResponse.objects.create(**validated_data)

        for query_response_data in query_responses_data:
            survey_response.query_responses.create(**query_response_data)

        return survey_response
