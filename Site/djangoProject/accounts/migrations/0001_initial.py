# Generated by Django 5.0.6 on 2024-06-04 10:39

import django.db.models.deletion
from django.conf import settings
from django.db import migrations, models


class Migration(migrations.Migration):

    initial = True

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
    ]

    operations = [
        migrations.CreateModel(
            name='Controller',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('imai', models.CharField(max_length=50, unique=True)),
                ('connection_pass', models.CharField(default='default_password', max_length=128)),
                ('user', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, null=True, blank=True, related_name='controllers', to=settings.AUTH_USER_MODEL)),
            ],
            options={
                'db_table': 'controllers',
            },
        ),
        migrations.CreateModel(
            name='Record',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('timestamp', models.DateTimeField(auto_now_add=True)),
                ('temperature', models.DecimalField(decimal_places=2, max_digits=5)),
                ('humidity', models.DecimalField(decimal_places=2, max_digits=5)),
                ('weight', models.DecimalField(decimal_places=2, max_digits=6)),
                ('voltage', models.DecimalField(decimal_places=2, max_digits=4)),
                ('sound_frequencies', models.TextField()),
                ('controller', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='accounts.controller')),
            ],
            options={
                'db_table': 'records',
            },
        ),
    ]
