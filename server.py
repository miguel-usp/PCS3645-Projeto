from flask import Flask, render_template, request, jsonify

PAGE_PATH = 'index.html'

app = Flask(__name__)


@app.route('/')
def index():
    return render_template(PAGE_PATH)


@app.route('/dirs', methods=['POST'])
def dirs():
    data = request.get_json()
    if data is None or not isinstance(data, list):
        return jsonify({'error': 'Invalid input. Expected a list of numbers in JSON format.'}), 400

    # Process the list of numbers here
    # For example, you can calculate the sum of the numbers
    result = sum(data)

    return jsonify({'result': result})


if __name__ == '__main__':
    app.run(debug=True)


if __name__ == '__main__':
    app.run(debug=True)
