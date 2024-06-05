from django.contrib.auth import login, authenticate
from django.contrib.auth.forms import AuthenticationForm
from django.shortcuts import render, redirect

from accounts.forms import SignUpForm

from django.contrib.auth.decorators import login_required
from accounts.forms import ControllerForm
from accounts.models import Controller
from django.contrib import messages
from django.http import HttpResponseRedirect

from django.contrib.auth import logout
#@login_required

def home(request):
    #if not request.user.is_authenticated:
    #    return HttpResponseRedirect('/login')
    return render(request, 'home.html')

def signup(request):
    if request.user.is_authenticated:
        return HttpResponseRedirect('/')
    if request.method == 'POST':
        form = SignUpForm(request.POST)
        if form.is_valid():
            form.save()
            username = form.cleaned_data.get('username')
            raw_password = form.cleaned_data.get('password1')
            user = authenticate(username=username, password=raw_password)
            login(request, user)
            return redirect('home')
    else:
        form = SignUpForm()
    return render(request, 'accounts/signup.html', {'form': form})

def login_view(request):
    if request.user.is_authenticated:
        return HttpResponseRedirect('/')
    if request.method == 'POST':
        form = AuthenticationForm(request, data=request.POST)
        if form.is_valid():
            username = form.cleaned_data.get('username')
            password = form.cleaned_data.get('password')
            user = authenticate(username=username, password=password)
            if user is not None:
                login(request, user)
                return redirect('home')
    else:
        form = AuthenticationForm()
    return render(request, 'accounts/login.html', {'form': form})

def logout_view(request):
    logout(request)
    return redirect('home')  # Перенаправление на главную страницу после выхода

@login_required
def add_controller(request):
    if request.user.is_superuser:
        messages.error(request, 'Администратору не разрешено добавлять контроллеры.')
        return redirect('home')

    if request.method == 'POST':
        form = ControllerForm(request.POST)
        if form.is_valid():
            imai = form.cleaned_data['imai']
            password = form.cleaned_data['connection_pass']

            try:
                # Поиск контроллера по логину
                controller = Controller.objects.get(imai=imai)

                print(f"controller.user = {controller.user}")
                # Проверка пароля
                if controller.password == password:
                    if controller.user is None or controller.user is "":
                        # Привязка пользователя к контроллеру
                        controller.user = request.user
                        controller.save()
                        messages.success(request, 'Контроллер успешно добавлен.')
                        return redirect('home')
                    else:
                        form.add_error('imai', 'Этот контроллер уже привязан к другому пользователю.')
                else:
                    form.add_error('connection_pass', 'Неправильный пароль.')
            except Controller.DoesNotExist:
                form.add_error('imai', 'Контроллер с таким логином не существует.')
        if not form.is_valid():
            print(form.errors)
    else:
        form = ControllerForm()

    return render(request, 'accounts/add_controller.html', {'form': form})