from core.views.survey_views import (
    get_messages_for_delegate,
    get_signup_state,
    get_surveys,
    message_to_delegate,
    post_aggregation_result,
    save_survey_response,
    signup_to_survey,
)
from django.contrib import admin
from django.urls import path

urlpatterns = [
    path("api/survey-response/", save_survey_response, name="survey-response"),
    path("api/surveys/", get_surveys, name="get-surveys"),
    path("api/survey-signup/", signup_to_survey, name="survey-signup"),
    path(
        "api/signup-state/<uuid:client_id>/",
        get_signup_state,
        name="get-signup-state",
    ),
    path(
        "api/message-to-delegate/",
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
