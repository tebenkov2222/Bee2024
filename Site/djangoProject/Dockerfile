FROM python:3.9-slim

WORKDIR /app

# Установить зависимости для компиляции пакетов и PostgreSQL
RUN apt-get update && apt-get install -y \
    build-essential \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

# Скопировать файл зависимостей
COPY requirements.txt /app/

# Установить зависимости Python
RUN pip install --no-cache-dir -r requirements.txt

# Скопировать весь код проекта
COPY . /app

# Установить переменную окружения для настроек Django
ENV DJANGO_SETTINGS_MODULE=djangoProject.settings

# Собрать статические файлы
RUN python manage.py collectstatic --noinput

# Открыть порт 8000 для Gunicorn
EXPOSE 8000

# Запустить приложение с Gunicorn
CMD ["gunicorn", "--bind", "0.0.0.0:8000", "djangoProject.wsgi:application"]
