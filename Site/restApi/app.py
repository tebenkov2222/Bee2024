from flask import Flask, request, jsonify
import secrets
import string
import pika
import json
from dotenv import load_dotenv
import os

# Загрузка переменных окружения из файла .env
load_dotenv()

app = Flask(__name__)

available_passwords = []

rabbitmq_host = os.getenv('RABBITMQ_HOST')
rabbitmq_port = os.getenv('RABBITMQ_PORT')
rabbitmq_user = os.getenv('RABBITMQ_USER')
rabbitmq_password = os.getenv('RABBITMQ_PASSWORD')
rabbitmq_queue = os.getenv('RABBITMQ_QUEUE')

def generate_password(length=15):
    characters = string.ascii_letters + string.digits + string.punctuation
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
    return password, 200


@app.route('/transmitdata', methods=['POST'])
def transmitdata_route():
    data = request.get_json()  # Для обработки JSON данных
    password = None
    login = None
    records = None

    if data:
        login = data.get('login')
        password = data.get('password')
        records = data.get('records')
    else:
        password = request.form.get('password')
        login = request.form.get('login')
        records = request.form.get('records')

    if password in available_passwords:
        available_passwords.remove(password)
        print(f"Login: {login}")
        print(f"Password: {password}")
        print(f"Records: {records}")

        # Отправка логина и записей в RabbitMQ
        send_to_rabbitmq(login, records)

        return "Data received and printed in console, and sent to RabbitMQ", 200
    else:
        return "Error: Invalid password", 403


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001, debug=True)
