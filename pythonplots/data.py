import requests

def get_data():
    res = requests.get('http://localhost:80')
    data = res.json()
    return data
