from django.contrib import admin

from .models.aggregation_group import AggregationGroup
from .models.client_to_delegate_message import ClientToDelegateMessage
from .models.commissioner import Commissioner
from .models.data_point import DataPoint, Types
from .models.response import QueryResponse, SurveyResponse
from .models.survey import Query, Survey
from .models.survey_signup import SurveySignup


@admin.register(Commissioner)
class CommissionerAdmin(admin.ModelAdmin):
    list_display = ("name",)
    search_fields = ("name",)


class QueryInline(admin.TabularInline):
    model = Query
    extra = 1
    readonly_fields = ["aggregated_results"]


@admin.register(Survey)
class SurveyAdmin(admin.ModelAdmin):
    list_display = ("name", "commissioner")
    inlines = [QueryInline]
    readonly_fields = ["id", "number_participants"]


class QueryResponseInline(admin.TabularInline):
    model = QueryResponse
    fields = ["data"]
    readonly_fields = ["data"]
    extra = 1


@admin.register(SurveyResponse)
class SurveyResponseAdmin(admin.ModelAdmin):
    inlines = [QueryResponseInline]


@admin.register(SurveySignup)
class SurveySignupAdmin(admin.ModelAdmin):
    list_display = ("id", "public_key", "survey", "time")
    list_display_links = ("id", "survey")
    list_filter = ("survey", "time")
    search_fields = ("id", "survey__name")
    readonly_fields = ("id", "public_key", "survey", "time", "group")


@admin.register(AggregationGroup)
class AggregationGroupAdmin(admin.ModelAdmin):
    list_display = ("id", "survey", "delegate")
    search_fields = ("survey__name", "delegate__name")
    readonly_fields = ("id", "delegate")


@admin.register(ClientToDelegateMessage)
class ClientToDelegateMessageAdmin(admin.ModelAdmin):
    list_display = ("id", "delegate_id", "group", "content")
    list_display_links = ["id"]
    list_filter = ("group", "content")
    search_fields = ("id", "delegate_id", "group", "content")


@admin.register(DataPoint)
class DataPointAdmin(admin.ModelAdmin):
    list_display = (
        "name",
        "key",
        "type_display",
        "min_value",
        "max_value",
        "precision",
    )
    list_filter = ("type",)
    search_fields = ("name",)
    ordering = ("name",)

    fieldsets = (
        (None, {"fields": ("name", "key", "type")}),
        ("Value Range", {"fields": ("min_value", "max_value")}),
        ("Precision", {"fields": ("precision",)}),
    )

    def type_display(self, obj):
        return obj.get_type_display()

    def formfield_for_choice_field(self, db_field, request, **kwargs):
        if db_field.name == "type":
            kwargs["choices"] = [(t.value, t.name) for t in Types]
        return super().formfield_for_choice_field(db_field, request, **kwargs)
