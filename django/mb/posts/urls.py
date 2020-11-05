from django.urls import path

from .views import HomePageView, MessageDetailView, MessageCreateView, MessageUpdateView, MessageDeleteView

urlpatterns = [
    path('message/<int:pk>/delete/', # new
        MessageDeleteView.as_view(), name='post_delete'),
    path('message/<int:pk>/edit/',MessageUpdateView.as_view(),name='post_edit'),
    path('message/new/', MessageCreateView.as_view(), name='post_new'),
    path('message/<int:pk>/', MessageDetailView.as_view(),name='post_detail'),
    path('',HomePageView.as_view(), name='home'),
]
