import json
from datetime import datetime, timedelta

from django.contrib.auth import login, authenticate
from django.contrib.auth.forms import AuthenticationForm
import string
import random
from django.shortcuts import render, redirect, get_object_or_404
from django.utils import timezone

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

            # Проверка, существует ли контроллер с таким IMEI
            if Controller.objects.filter(imai=imai).exists():
                form.add_error(None, 'Контроллер с таким IMEI уже существует.')
            else:
                # Генерация случайного пароля из 8 символов
                password = ''.join(random.choices(string.ascii_letters + string.digits, k=8))
                # Создание нового контроллера
                Controller.objects.create(imai=imai, connection_pass=password)
                messages.success(request, f'Новый контроллер успешно добавлен. Сгенерированный пароль: {password}')
                form = ControllerRegistrationForm()  # Очистка формы после успешного добавления
    else:
        form = ControllerRegistrationForm()

    controllers = Controller.objects.all()
    return render(request, 'accounts/registration_controller.html', {'form': form, 'controllers': controllers})


@login_required
def user_controllers(request):
    user = request.user
    controllers = Controller.objects.filter(user=user)
    data = []

    now = timezone.now()  # !!! timezone.now() вместо datetime.now() !!!
    start_time_today = now.replace(hour=4, minute=0, second=0, microsecond=0)
    start_time_yesterday = start_time_today - timedelta(days=1)

    for controller in controllers:
        latest_record = Record.objects.filter(controller=controller).order_by('-timestamp').first()

        # Используем timezone-aware datetime объекты
        record_today_4am = Record.objects.filter(controller=controller, timestamp__gte=start_time_today).order_by(
            'timestamp').first()
        record_yesterday_4am = Record.objects.filter(controller=controller, timestamp__gte=start_time_yesterday,
                                                     timestamp__lt=start_time_today).order_by('timestamp').first()

        delta_mass = None
        if record_today_4am and record_yesterday_4am:
            delta_mass = round(record_today_4am.weight - record_yesterday_4am.weight, 2)

        data.append({
            'controller': controller,
            'latest_record': latest_record,
            'delta_mass': delta_mass if delta_mass is not None else 'Недоступно '
        })

    return render(request, 'accounts/user_controllers.html', {'data': data})


@login_required
def controller_details(request, controller_id):
    one_week_ago = datetime.now() - timedelta(days=7)
    controller = get_object_or_404(Controller, id=controller_id, user=request.user)
    records = list(Record.objects.filter(controller=controller, timestamp__gte=one_week_ago).values(
        'timestamp', 'temperature', 'humidity', 'outside_temperature', 'outside_humidity',
        'weight', 'voltage', 'signal_strength'
    ))
    serialized_records = [
        {
            'timestamp': record['timestamp'].isoformat(),
            'temperature': float(record['temperature']),
            'humidity': float(record['humidity']),
            'outside_temperature': float(record['outside_temperature']),
            'outside_humidity': float(record['outside_humidity']),
            'weight': float(record['weight']),
            'voltage': float(record['voltage']),
            'signal_strength': record['signal_strength'],
        }
        for record in records
    ]
    last_record = serialized_records[-1] if serialized_records else None
    return render(request, 'accounts/controller_details.html',
                  {'controller': controller, 'records': json.dumps(serialized_records), 'last_record': last_record})