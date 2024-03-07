import json

from core.models import QueryResponse
from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods


@csrf_exempt
@require_http_methods(["POST"])
def save_data(request):
    try:
        data = json.loads(request.body)
        json_data = QueryResponse.objects.create(data=data)
        json_data.save()
        return JsonResponse(data, status=201)
    except json.JSONDecodeError:
        return HttpResponse("Invalid JSON", status=400)
