from django.shortcuts import render
from django.views.generic import ListView, DetailView
from django.views.generic.edit import CreateView, UpdateView, DeleteView
from django.urls import reverse_lazy

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

class MessageUpdateView(UpdateView):
    model = Post
    template_name = 'message_edit.html'
    fields = ['title', 'text']

class MessageDeleteView(DeleteView):
    model = Post
    template_name = 'message_delete.html'
    success_url = reverse_lazy('home')
