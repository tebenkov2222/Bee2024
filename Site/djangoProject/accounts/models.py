from django.db import models
from django.contrib.auth.models import User

class Controller(models.Model):
    imai = models.CharField(max_length=50, null=False, name='imai')
    connection_pass = models.CharField(max_length=128, null=False, default='default_password', name='connection_pass')
    user = models.ForeignKey(User, on_delete=models.CASCADE, related_name='controllers')

    class Meta:
        db_table = 'controllers'  # Указываем имя таблицы

class Record(models.Model):
    controller = models.ForeignKey(Controller, on_delete=models.CASCADE)
    timestamp = models.DateTimeField(auto_now_add=True)
    temperature = models.DecimalField(max_digits=5, decimal_places=2)
    humidity = models.DecimalField(max_digits=5, decimal_places=2)
    weight = models.DecimalField(max_digits=6, decimal_places=2)
    voltage = models.DecimalField(max_digits=4, decimal_places=2)
    sound_frequencies = models.TextField()

    class Meta:
        db_table = 'records'  # Указываем имя таблицы