# Используем базовый образ Python
FROM python:3.9-slim

ENV PYTHONUNBUFFERED=1
# Устанавливаем рабочую директорию
WORKDIR /app

# Копируем requirements.txt в рабочую директорию
COPY requirements.txt /app/

# Устанавливаем зависимости
RUN pip install --no-cache-dir -r requirements.txt

# Копируем весь код приложения в рабочую директорию
COPY . /app

# Открываем порт, на котором будет работать наше приложение
EXPOSE 5001

# Задаем команду для запуска приложения
CMD ["python", "app.py"]