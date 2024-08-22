from rest_framework import serializers

from .models.commissioner import Commissioner
from .models.data_point import DataPoint
from .models.response import QueryResponse, SurveyResponse
from .models.survey import Query, Survey


class CommissionerSerializer(serializers.ModelSerializer):
    class Meta:
        model = Commissioner
        fields = ("name",)


class DataPointSerializer(serializers.ModelSerializer):
    class Meta:
        model = DataPoint
        fields = ["id", "name"]


# This has gotten a little out of hand as we keep the old data_key logic in the
# client, while we want the data_point logic on the server to be able to control
# data points being surveyed
class QuerySerializer(serializers.ModelSerializer):
    data_key = serializers.CharField(write_only=True)
    discrete = serializers.BooleanField()

    class Meta:
        model = Query
        fields = ["id", "data_key", "cohorts", "discrete"]
        read_only_fields = ["id"]

    def to_representation(self, instance):
        representation = super().to_representation(instance)
        representation["data_key"] = instance.data_point.to_data_key()
        return representation

    def validate_data_key(self, value):
        try:
            data_point = DataPoint.objects.filter(name__iexact=value).first()
            if data_point is None:
                raise serializers.ValidationError(
                    f"No DataPoint found for data_key: {value}"
                )
            return data_point
        except Exception as e:
            raise serializers.ValidationError(str(e))

    def create(self, validated_data):
        data_point = validated_data.pop("data_key")
        return Query.objects.create(data_point=data_point, **validated_data)

    def update(self, instance, validated_data):
        if "data_key" in validated_data:
            instance.data_point = validated_data.pop("data_key")
        for attr, value in validated_data.items():
            setattr(instance, attr, value)
        instance.save()
        return instance


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
