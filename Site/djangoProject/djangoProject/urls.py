"""
URL configuration for djangoProject project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/5.0/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path, include

from accounts.views import signup, login_view, add_controller, home, logout_view
from .views import admin_view

urlpatterns = [
    path('admin/', admin.site.urls),
    #path('auth/', include('django.contrib.auth.urls')),  # Включает встроенные URL для входа/выхода,
    path('admin-view/', admin_view, name='admin_view'),
    path('signup/', signup, name='signup'),
    path('login/', login_view, name='login'),
    path('logout/', logout_view, name='logout'),
    path('logout/', login_view, name='logout'),
    path('add_controller/', add_controller, name='add_controller'),
    path('', home, name='home'),
]