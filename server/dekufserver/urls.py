"""
URL configuration for dekufserver project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/5.0/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""

from core.views import (
    get_messages_for_delegate,
    get_signup_state,
    get_surveys,
    message_to_delegate,
    save_survey_response,
    signup_to_survey,
    post_aggregation_result,
)
from django.contrib import admin
from django.urls import path

urlpatterns = [
    path("admin/", admin.site.urls),
    path("api/survey-response/", save_survey_response, name="survey-response"),
    path("api/surveys/", get_surveys, name="get-surveys"),
    path(
        "api/survey-signup/<uuid:survey_id>/",
        signup_to_survey,
        name="survey-signup",
    ),
    path(
        "api/signup-state/<uuid:client_id>/",
        get_signup_state,
        name="get-signup-state",
    ),
    path(
        "api/message-to-delegate/<uuid:delegate_id>/",
        message_to_delegate,
        name="message-to-delegate",
    ),
    path(
        "api/messages-for-delegate/<uuid:delegate_id>/",
        get_messages_for_delegate,
        name="get-messages-for-delegate",
    ),
    path(
        "api/post-aggregation-result/<uuid:delegate_id>/",
        post_aggregation_result,
        name="post-aggregation-result",
    ),
]
