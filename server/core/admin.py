from django.contrib import admin

from .models.commissioner import Commissioner
from .models.response import QueryResponse, SurveyResponse
from .models.survey import Query, Survey


@admin.register(Commissioner)
class CommissionerAdmin(admin.ModelAdmin):
    list_display = ("name",)
    search_fields = ("name",)


class QueryInline(admin.TabularInline):
    model = Query
    extra = 1
    readonly_fields = ("aggregated_results", "number_participants")


@admin.register(Survey)
class SurveyAdmin(admin.ModelAdmin):
    list_display = ("name", "commissioner")
    inlines = [QueryInline]


class QueryResponseInline(admin.TabularInline):
    model = QueryResponse
    fields = ["data"]
    readonly_fields = ["data"]
    extra = 1


@admin.register(SurveyResponse)
class SurveyResponseAdmin(admin.ModelAdmin):
    inlines = [QueryResponseInline]
