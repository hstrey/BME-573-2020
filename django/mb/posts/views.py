from django.shortcuts import render
from django.views.generic import ListView, DetailView
from django.views.generic.edit import CreateView

from .models import Post

# Create your views here.
class HomePageView(ListView):
    model = Post
    template_name = 'home.html'
    context_object_name = 'all_posts_list'

class MessageDetailView(DetailView):
    model = Post
    template_name = 'message_detail.html'

class MessageCreateView(CreateView):
    model = Post
    template_name = 'message_new.html'
    fields = ['title','author','text']

