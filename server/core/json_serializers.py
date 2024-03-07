from rest_framework import serializers

from .models import Commissioner, QueryResponse, SurveyResponse


class CommissionerSerializer(serializers.ModelSerializer):
    class Meta:
        model = Commissioner
        fields = ("name",)


class QueryResponseSerializer(serializers.ModelSerializer):
    class Meta:
        model = QueryResponse
        fields = ("data_key", "data")


class SurveyResponseSerializer(serializers.ModelSerializer):
    commissioners = CommissionerSerializer(many=True)
    queryResponses = QueryResponseSerializer(
        many=True
    )

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
