import json

from rest_framework.parsers import JSONParser

from core.json_serializers import SurveyResponseSerializer
from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods


@csrf_exempt
@require_http_methods(["POST"])
def save_response(request):
    try:
        survey_response_data = JSONParser().parse(request)

        serializer = SurveyResponseSerializer(data=survey_response_data)

        if serializer.is_valid():
            survey_response = serializer.save()
            return JsonResponse(serializer.data, status=201)
        else:
            return JsonResponse(serializer.errors, status=400)
    except json.JSONDecodeError:
        return HttpResponse("Invalid JSON", status=400)
