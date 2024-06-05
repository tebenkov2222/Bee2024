import json
import pika
import psycopg2
import os
from dotenv import load_dotenv
from datetime import datetime

# Загрузка переменных окружения из файла .env
load_dotenv()

# Настройки подключения к PostgreSQL
db_host = os.getenv('DB_HOST')
db_port = os.getenv('DB_PORT')
db_name = os.getenv('DB_NAME')
db_user = os.getenv('DB_USER')
db_password = os.getenv('DB_PASSWORD')

# Настройки подключения к RabbitMQ
rabbitmq_host = os.getenv('RABBITMQ_HOST')
rabbitmq_port = os.getenv('RABBITMQ_PORT')
rabbitmq_user = os.getenv('RABBITMQ_USER')
rabbitmq_password = os.getenv('RABBITMQ_PASSWORD')
rabbitmq_queue = os.getenv('RABBITMQ_QUEUE')

# Функция для получения ID контроллера по login, добавляет контроллер, если его нет
def get_controller_id(login):
    try:
        connection = psycopg2.connect(
            host=db_host,
            port=db_port,
            database=db_name,
            user=db_user,
            password=db_password
        )
        cursor = connection.cursor()
        cursor.execute("SELECT id FROM controllers WHERE login = %s", (login,))
        result = cursor.fetchone()
        if result:
            controller_id = result[0]
        else:
            cursor.execute("INSERT INTO controllers (login) VALUES (%s) RETURNING id", (login,))
            controller_id = cursor.fetchone()[0]
            connection.commit()
        cursor.close()
        connection.close()
        return controller_id
    except Exception as e:
        print(f"Failed to get or insert controller: {e}")
        return None

# Функция для записи распарсенного сообщения в базу данных
def save_to_db(controller_id, timestamp, temperature, humidity, weight, voltage, sound_frequencies):
    try:
        connection = psycopg2.connect(
            host=db_host,
            port=db_port,
            database=db_name,
            user=db_user,
            password=db_password
        )
        cursor = connection.cursor()
        cursor.execute("""
            INSERT INTO records (controller_id, timestamp, temperature, humidity, weight, voltage, sound_frequencies)
            VALUES (%s, %s, %s, %s, %s, %s, %s)
        """, (controller_id, timestamp, temperature, humidity, weight, voltage, sound_frequencies))
        connection.commit()
        cursor.close()
        connection.close()
        print(f"Record saved to database: {timestamp}, {temperature}, {humidity}, {weight}, {voltage}, {sound_frequencies}")
    except Exception as e:
        print(f"Failed to save record to database: {e}")

# Функция для обработки сообщений из RabbitMQ
def callback(ch, method, properties, body):
    message = body.decode('utf-8')
    print(f"Received message: {message}")

    try:
        data = json.loads(message)
        login = data['login']
        records = data['records'].split(';')

        timestamp = datetime.now()
        temperature = float(records[1])
        humidity = float(records[2])
        weight = float(records[3])
        voltage = float(records[4])
        sound_frequencies = records[5]

        controller_id = get_controller_id(login)
        if controller_id:
            save_to_db(controller_id, timestamp, temperature, humidity, weight, voltage, sound_frequencies)
    except Exception as e:
        print(f"Failed to parse or save message: {e}")

# Настройка подключения к RabbitMQ
credentials = pika.PlainCredentials(rabbitmq_user, rabbitmq_password)
parameters = pika.ConnectionParameters(rabbitmq_host, rabbitmq_port, '/', credentials)

connection = pika.BlockingConnection(parameters)
channel = connection.channel()

# Объявление очереди с durable=True
channel.queue_declare(queue=rabbitmq_queue, durable=True)

# Подписка на очередь
channel.basic_consume(queue=rabbitmq_queue, on_message_callback=callback, auto_ack=True)

print('Waiting for messages. To exit press CTRL+C')
channel.start_consuming()
