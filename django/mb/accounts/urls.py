from django.urls import path

from .views import SignUpView

urlpatterns = [
    path('signup/', SignupView.as_view(), name='signup'),
]
