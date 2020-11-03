from django.db import models
from django.urls import reverse

# Create your models here.
class Post(models.Model):
    title = models.CharField(max_length=200, default="No Title")
    author = models.ForeignKey('auth.User',
    default=1,
    on_delete=models.CASCADE,
    )
    text = models.TextField()

    def __str__(self):
        return self.title[:50]
    
    def get_absolute_url(self):
        return reverse('post_detail', args=[str(self.id)])