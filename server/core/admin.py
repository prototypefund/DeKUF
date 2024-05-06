from django.contrib import admin

from .models.commissioner import Commissioner
from .models.survey import  Query, Survey
from .models.response import SurveyResponse, QueryResponse


@admin.register(Commissioner)
class CommissionerAdmin(admin.ModelAdmin):
    list_display = ("name",)
    search_fields = ("name",)


class QueryInline(admin.TabularInline):
    model = Query
    extra = 1


@admin.register(Survey)
class SurveyAdmin(admin.ModelAdmin):
    list_display = ("name", "commissioner")
    inlines = [QueryInline]


class QueryResponseInline(admin.TabularInline):
    model = QueryResponse
    fields = ("data_key", "data")
    readonly_fields = ("data_key", "data")
    extra = 1


@admin.register(SurveyResponse)
class SurveyResponseAdmin(admin.ModelAdmin):
    inlines = [QueryResponseInline]
