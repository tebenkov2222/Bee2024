from django.contrib.auth.forms import UserCreationForm
from django.contrib.auth.models import User
from accounts.models import Controller
from django import forms

class SignUpForm(UserCreationForm):
    email = forms.EmailField(max_length=200, help_text='Required')

    class Meta:
        model = User
        fields = ('username', 'email', 'password1', 'password2')


class ControllerForm(forms.ModelForm):
    class Meta:
        model = Controller
        fields = ['imai', 'connection_pass']
        widgets = {
            'connection_pass': forms.PasswordInput(),
        }

class ControllerRegistrationForm(forms.ModelForm):
    class Meta:
        model = Controller
        fields = ['imai']
