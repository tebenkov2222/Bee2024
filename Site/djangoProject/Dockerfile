# Используем базовый образ Python
FROM python:3.9-slim

# Устанавливаем рабочую директорию
WORKDIR /app

# Устанавливаем зависимости системы
RUN apt-get update && apt-get install -y \
    build-essential \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

# Копируем requirements.txt в рабочую директорию
COPY requirements.txt /app/

# Устанавливаем зависимости Python
RUN pip install --no-cache-dir -r requirements.txt

# Копируем весь код проекта в рабочую директорию
COPY . /app

# Задаем переменную окружения для Django
ENV DJANGO_SETTINGS_MODULE=djangoProject.settings

# Открываем порт, на котором будет работать наше приложение
EXPOSE 8000

# Выполняем команду миграции
#RUN python manage.py migrate

# Запускаем команду для сборки статики (если нужно)
#RUN python manage.py collectstatic --noinput

# Задаем команду для запуска приложения
CMD ["gunicorn", "--bind", "0.0.0.0:8000", "djangoProject.wsgi:application"]
