import re

from django.contrib.auth.forms import UserCreationForm
from django.contrib.auth.models import User
from accounts.models import Controller
from django import forms


class SignUpForm(UserCreationForm):
    email = forms.EmailField(required=True, widget=forms.EmailInput(attrs={'class': 'form-control'}))
    username = forms.CharField(max_length=150, widget=forms.TextInput(attrs={'class': 'form-control'}))
    password1 = forms.CharField(widget=forms.PasswordInput(attrs={'class': 'form-control'}))
    password2 = forms.CharField(widget=forms.PasswordInput(attrs={'class': 'form-control'}))

    class Meta:
        model = User
        fields = ('username', 'email', 'password1', 'password2')


class ControllerForm(forms.ModelForm):
    class Meta:
        model = Controller
        fields = ['imai', 'connection_pass']
        labels = {
            'imai': 'IMEI:',
            'connection_pass': 'Пароль подключения:',
        }
        widgets = {
            'connection_pass': forms.PasswordInput(),
        }

class ControllerRegistrationForm(forms.ModelForm):
    class Meta:
        model = Controller
        fields = ['imai']
        labels = {
            'imai': 'IMEI:',
        }

    def clean_imai(self):
        imai = self.cleaned_data.get('imai')
        if not re.match(r'^\d{14}(\d{1}|\d{3})$', imai):
            raise forms.ValidationError('IMEI должен содержать либо 15 цифр, либо 17 цифр.')
        return imai