import json
import pika
import psycopg2
import os
from dotenv import load_dotenv
from datetime import datetime
import logging

# Загрузка переменных окружения из файла .env
load_dotenv()

# Настройка логгирования
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

logger.info('Start program')

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
        cursor.execute("SELECT id FROM controllers WHERE imai = %s", (login,))
        result = cursor.fetchone()
        if result:
            controller_id = result[0]
        else:
            cursor.execute("INSERT INTO controllers (imai) VALUES (%s) RETURNING id", (login,))
            controller_id = cursor.fetchone()[0]
            connection.commit()
        cursor.close()
        connection.close()
        return controller_id
    except Exception as e:
        logger.error(f"Failed to get or insert controller: {e}")
        return None

# Функция для записи распарсенного сообщения в базу данных
def save_to_db(controller_id, timestamp, data):
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
            INSERT INTO records (controller_id, timestamp, humidity, temperature, outside_humidity, outside_temperature, weight, voltage, battery_current, battery_power, solar_voltage, solar_current, solar_power, signal_strength, last_pass_status, last_data_transfer_status, sound_frequencies)
            VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
        """, (
            controller_id, timestamp, data['humidity_inside'], data['temperature_inside'], data['humidity_outside'],
            data['temperature_outside'], data['weight'], data['battery_voltage'], data['battery_current'],
            data['battery_power'], data['solar_voltage'], data['solar_current'], data['solar_power'],
            data['signal_strength'], data['last_pass_status'], data['last_data_transfer_status'], data['sound_frequencies']
        ))
        connection.commit()
        cursor.close()
        connection.close()
        logger.info(f"Record saved to database: {data}")
    except Exception as e:
        logger.error(f"Failed to save record to database: {e}")

# Функция для обработки сообщений из RabbitMQ
def callback(ch, method, properties, body):
    message = body.decode('utf-8')
    logger.info(f"Received message: {message}")

    try:
        data = json.loads(message)
        login = data['login']
        records = data['records'].split(';')

        timestamp = datetime.now()
        parsed_data = {
            'humidity_inside': float(records[1]),
            'temperature_inside': float(records[2]),
            'humidity_outside': float(records[3]),
            'temperature_outside': float(records[4]),
            'weight': float(records[5]),
            'battery_voltage': float(records[6]),
            'battery_current': float(records[7]),
            'battery_power': float(records[8]),
            'solar_voltage': float(records[9]),
            'solar_current': float(records[10]),
            'solar_power': float(records[11]),
            'signal_strength': int(records[12]),
            'last_pass_status': int(records[13]),
            'last_data_transfer_status': int(records[14]),
            'sound_frequencies': records[15]
        }

        controller_id = get_controller_id(login)
        if controller_id:
            save_to_db(controller_id, timestamp, parsed_data)
    except Exception as e:
        logger.error(f"Failed to parse or save message: {e}")

if __name__ == '__main__':
    # Настройка подключения к RabbitMQ
    credentials = pika.PlainCredentials(rabbitmq_user, rabbitmq_password)
    parameters = pika.ConnectionParameters(rabbitmq_host, rabbitmq_port, '/', credentials)

    connection = pika.BlockingConnection(parameters)
    channel = connection.channel()

    # Объявление очереди с durable=True
    channel.queue_declare(queue=rabbitmq_queue, durable=True)

    # Подписка на очередь
    channel.basic_consume(queue=rabbitmq_queue, on_message_callback=callback, auto_ack=True)

    logger.info('Waiting for messages. To exit press CTRL+C')
    channel.start_consuming()
