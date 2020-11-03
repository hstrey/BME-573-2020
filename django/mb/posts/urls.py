from django.urls import path

from .views import HomePageView, MessageDetailView, MessageCreateView

urlpatterns = [
    path('message/new/', MessageCreateView.as_view(), name='post_new'),
    path('message/<int:pk>/', MessageDetailView.as_view(),name='post_detail'),
    path('',HomePageView.as_view(), name='home'),
]
