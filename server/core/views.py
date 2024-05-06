import json

from core.json_serializers import SurveyResponseSerializer, SurveySerializer
from core.models.survey import Survey
from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods
from rest_framework.parsers import JSONParser


@csrf_exempt
@require_http_methods(["POST"])
def save_survey_response(request):
    try:
        survey_response_data = JSONParser().parse(request)

        serializer = SurveyResponseSerializer(data=survey_response_data)

        if serializer.is_valid():
            return JsonResponse(serializer.data, status=201)
        else:
            return JsonResponse(serializer.errors, status=400)
    except json.JSONDecodeError:
        return HttpResponse("Invalid JSON", status=400)


@require_http_methods(["GET"])
def get_surveys(request):
    surveys = Survey.objects.all().prefetch_related("commissioners")
    serializer = SurveySerializer(surveys, many=True)
    return JsonResponse(serializer.data, safe=False)
