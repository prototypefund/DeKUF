import json

from core.json_serializers import SurveyResponseSerializer, SurveySerializer
from core.models.grouping_logic import group_ungrouped_signups
from core.models.survey import Survey
from core.models.survey_signup import SurveySignup
from django.http import HttpResponse, JsonResponse
from django.shortcuts import get_object_or_404
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods
from rest_framework.parsers import JSONParser


@csrf_exempt
@require_http_methods(["POST"])
def save_survey_response(request):
    try:
        survey_response_data = JSONParser().parse(request)

        print(survey_response_data)
        serializer = SurveyResponseSerializer(data=survey_response_data)

        if serializer.is_valid():
            survey_response = serializer.save()
            for query_response in survey_response.query_responses.all():
                query_response.query.aggregate_query_response(query_response)
            return JsonResponse(serializer.data, status=201)
        else:
            return JsonResponse(serializer.errors, status=400)
    except json.JSONDecodeError:
        return HttpResponse("Invalid JSON", status=400)


@require_http_methods(["GET"])
def get_surveys(request):
    surveys = Survey.objects.all()
    serializer = SurveySerializer(surveys, many=True)
    return JsonResponse(serializer.data, safe=False)


@csrf_exempt
@require_http_methods(["POST"])
def signup_to_survey(request, survey_id):
    try:
        survey = get_object_or_404(Survey, id=survey_id)
        survey_signup = SurveySignup.objects.create(survey=survey)

        response_data = {
            "client_id": str(survey_signup.id),
            "survey_id": str(survey.id),
            "time": survey_signup.time.isoformat(),
        }

        group_ungrouped_signups(
            ungrouped_signups=list(
                SurveySignup.objects.filter(group__isnull=True)),
            survey=survey,
        )

        return JsonResponse(response_data, status=201)
    except json.JSONDecodeError:
        return HttpResponse("Invalid JSON", status=400)
