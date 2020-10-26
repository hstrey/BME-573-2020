from django.db import models

# Create your models here.
class Post(models.Model):
    title = models.CharField(max_length=50, default="No Title")
    author = models.ForeignKey('auth.User', default=1,
    on_delete=models.CASCADE,
    )
    text = models.TextField()

    def __str__(self):
        return self.text[:50]
        