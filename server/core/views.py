import json

from core.models import SurveyResponse
from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods


@csrf_exempt
@require_http_methods(["POST"])
def save_response(request):
    try:
        survey_response_data = json.loads(request.body)
        survey_response = SurveyResponse.objects.create(data=survey_response_data)
        survey_response.save()
        return JsonResponse(survey_response_data, status=201)
    except json.JSONDecodeError:
        return HttpResponse("Invalid JSON", status=400)
