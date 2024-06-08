from datetime import datetime, timedelta

from django.contrib.auth import login, authenticate
from django.contrib.auth.forms import AuthenticationForm
import string
import random
from django.shortcuts import render, redirect, get_object_or_404

from accounts.forms import SignUpForm

from django.contrib.auth.decorators import login_required
from accounts.forms import ControllerForm, ControllerRegistrationForm
from accounts.models import Controller, Record
from django.contrib import messages
from django.http import HttpResponseRedirect, JsonResponse

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
        return redirect('registration_controller')

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
                if controller.connection_pass == password:
                    if controller.user is None or controller.user == "":
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



@login_required
def registration_controller(request):
    if not request.user.is_superuser:
        messages.error(request, 'Пользователям не разрешено добавлять контроллеры.')
        return redirect('home')

    if request.method == 'POST':
        form = ControllerRegistrationForm(request.POST)
        if form.is_valid():
            imai = form.cleaned_data['imai']

            # Генерация случайного пароля из 8 символов
            password = ''.join(random.choices(string.ascii_letters + string.digits, k=8))

            # Проверка, существует ли контроллер с таким IMEI
            if Controller.objects.filter(imai=imai).exists():
                form.add_error('imai', 'Контроллер с таким IMEI уже существует.')
            else:
                # Создание нового контроллера
                Controller.objects.create(imai=imai, connection_pass=password)
                messages.success(request, f'Новый контроллер успешно добавлен. Сгенерированный пароль: {password}')
        else:
            print(form.errors)
    else:
        form = ControllerRegistrationForm()

    return render(request, 'accounts/registration_controller.html', {'form': form})

@login_required
def user_controllers(request):
    user = request.user
    controllers = Controller.objects.filter(user=user)
    data = []
    one_day_ago = datetime.now() - timedelta(days=6)

    for controller in controllers:
        latest_record = \
            Record.objects.filter(controller=controller, timestamp__gte=one_day_ago).order_by('-timestamp').first()
        if latest_record:
            data.append({
                'controller': controller,
                'latest_record': latest_record,
                'delta_mass': round((latest_record.weight) - 50, 2)  # Assuming 50 as a reference weight for demo
            })

    return render(request, 'accounts/user_controllers.html', {'data': data})

@login_required
def controller_details(request, controller_id):
    one_day_ago = datetime.now() - timedelta(days=6)
    controller = get_object_or_404(Controller, id=controller_id, user=request.user)
    records = Record.objects.filter(controller=controller, timestamp__gte=one_day_ago).values(
        'timestamp', 'temperature', 'humidity', 'weight', 'voltage'
    )
    serialized_records = [
        {
            'timestamp': record['timestamp'].isoformat(),
            'temperature': float(record['temperature']),
            'humidity': float(record['humidity']),
            'weight': float(record['weight']),
            'voltage': float(record['voltage']),
        }
        for record in records
    ]
    return render(request, 'accounts/controller_details.html', {'controller': controller, 'records': serialized_records})