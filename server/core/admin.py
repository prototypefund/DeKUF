from django.contrib import admin

from .models import Commissioner, Query, Survey, SurveyResponse, QueryResponse


@admin.register(Commissioner)
class CommissionerAdmin(admin.ModelAdmin):
    list_display = ("name",)
    search_fields = ("name",)


class QueryInline(admin.TabularInline):
    model = Query
    extra = 1


@admin.register(Survey)
class SurveyAdmin(admin.ModelAdmin):
    list_display = ("name",)
    inlines = [QueryInline]
    search_fields = ("commissioners__name",)
    filter_horizontal = ("commissioners",)


class QueryResponseInline(admin.TabularInline):
    model = QueryResponse
    fields = ('data_key', 'data')
    readonly_fields = ('data_key', 'data')
    extra = 1


@admin.register(SurveyResponse)
class SurveyResponseAdmin(admin.ModelAdmin):
    inlines = [QueryResponseInline]
