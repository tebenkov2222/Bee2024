import logging
from flask import Flask, request, jsonify
import secrets
import string
import pika
import json
from dotenv import load_dotenv
import os

# Загрузка переменных окружения из файла .env
load_dotenv()

# Настройка логгирования
logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s %(levelname)s %(message)s',
                    handlers=[logging.StreamHandler()])

app = Flask(__name__)

available_passwords = []

rabbitmq_host = os.getenv('RABBITMQ_HOST')
rabbitmq_port = os.getenv('RABBITMQ_PORT')
rabbitmq_user = os.getenv('RABBITMQ_USER')
rabbitmq_password = os.getenv('RABBITMQ_PASSWORD')
rabbitmq_queue = os.getenv('RABBITMQ_QUEUE')

def generate_password(length=15):
    characters = string.ascii_letters + string.digits
    password = ''.join(secrets.choice(characters) for i in range(length))
    return password

def send_to_rabbitmq(login, records):
    credentials = pika.PlainCredentials(rabbitmq_user, rabbitmq_password)
    parameters = pika.ConnectionParameters(rabbitmq_host, rabbitmq_port, '/', credentials)
    connection = pika.BlockingConnection(parameters)
    channel = connection.channel()

    # Объявление очереди
    channel.queue_declare(queue=rabbitmq_queue, durable=True)

    message = {
        'login': login,
        'records': records
    }

    channel.basic_publish(exchange='',
                          routing_key=rabbitmq_queue,
                          body=json.dumps(message))

    connection.close()

@app.route('/getpass', methods=['GET'])
def getpass_route():
    password = generate_password()
    available_passwords.append(password)
    app.logger.info(f"Generated Pass: |{password}|")

    return password, 200

@app.route('/transmitdata', methods=['POST'])
def transmitdata_route():
    # Получаем данные из запроса в формате application/x-www-form-urlencoded
    password = request.form.get('password')
    login = request.form.get('login')
    records = request.form.get('data')

    app.logger.info(f"Login: |{login}|")
    app.logger.info(f"Password: |{password}|")
    app.logger.info(f"Records: |{records}|")

    app.logger.debug(f"Available passwords: {available_passwords}")

    if password in available_passwords or True:
    #if len(login) > 0:

        send_to_rabbitmq(login, records)
        available_passwords.remove(password)

        # Отправка логина и записей в RabbitMQ

        return "Data received and printed in console, and sent to RabbitMQ", 200
    else:
        app.logger.warning("Invalid password attempt.")
        return "Error: Invalid password", 403

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001, debug=True)
