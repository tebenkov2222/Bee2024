# Используем официальный образ Python в качестве базового
FROM python:3.9-slim

# Устанавливаем зависимости для компиляции psycopg2
RUN apt update && apt install -y build-essential libpq-dev

# Устанавливаем переменную окружения для отключения буферизации
ENV PYTHONUNBUFFERED=1

# Устанавливаем рабочую директорию
WORKDIR /app

# Копируем файл зависимостей в контейнер
COPY requirements.txt /app/

# Устанавливаем зависимости проекта
RUN pip install --no-cache-dir -r requirements.txt

# Копируем все файлы проекта в контейнер
COPY . /app

# Выводим содержимое текущей директории (для отладки)
RUN ls -la /app

# Указываем команду для запуска приложения
CMD ["python", "main.py"]
