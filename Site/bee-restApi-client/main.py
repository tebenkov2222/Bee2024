import os
import requests
import schedule
import time
from dotenv import load_dotenv
from itertools import cycle
# Загрузка переменных окружения из файла .env
load_dotenv()

# Получение логина и URL из переменных окружения
login = os.getenv('LOGIN')
url = os.getenv('URL')

# URL для получения пароля
getpass_url = f'{url}/getpass'

# Переменная для хранения пароля
password = None

def load_records(file_path):
    global records_iterator
    with open(file_path, 'r') as file:
        records = file.readlines()
    records_iterator = cycle(records)
# Функция для получения пароля
def fetch_password():
    global password
    response = requests.get(getpass_url)
    if response.status_code == 200:
        password = response.text
        print("Password fetched successfully")
    else:
        print(f"Failed to fetch password: {response.status_code}")


# Функция для отправки данных
def send_data():
    global password
    if not password:
        print("Password is not set. Cannot send data.")
        return

    #records = "0:0:19;35.10;19.60;62.05;5.00;[27,19,0,19,19,19,0,30,19,19,19,19,19,19,0,8];"
    records = next(records_iterator).strip()
    data = {
        'login': login,
        'password': password,
        'records': records
    }

    response = requests.post(f'{url}/transmitdata', json=data)

    if response.status_code == 200:
        print("Data sent successfully")
    else:
        print(f"Failed to send data: {response.status_code}")

def get_pass_and_send_data():
    fetch_password()
    send_data()


# Планирование задачи на каждые 10 минут
schedule.every(1).minutes.do(get_pass_and_send_data)

print("Client started. Sending data every 10 minutes.")

load_records('data.txt')
get_pass_and_send_data()
# Бесконечный цикл для выполнения задач по расписанию

if __name__ == '__main__':
    while True:
        schedule.run_pending()
        time.sleep(1)