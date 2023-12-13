import random
from flask import Flask, render_template, request, jsonify
import requests

PAGE_PATH = 'index.html'

app = Flask(__name__)

VEHICLES_IPS = ['192.168.68.27']


@app.route('/')
def index():
    return render_template(PAGE_PATH)


@app.route('/dirs', methods=['POST'])
def dirs():
    data = request.get_json()
    if data is None:
        return jsonify({'result': 'Error: no data provided'}), 400
    for ri in data:
        print(ri)
        num_stall = random.randint(0, 4)
        requests.post('http://' + VEHICLES_IPS[0] + '/dirs',
                      data=f"{('s' * num_stall) + ''.join(map(str, ri['directions']))}")

    return jsonify({'result': 'OK'}), 200


if __name__ == '__main__':
    app.run(debug=True)


if __name__ == '__main__':
    app.run(debug=True)
