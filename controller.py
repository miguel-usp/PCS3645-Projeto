
import requests
import csv
import io

import directions

CAR_IP = "192.168.200.234"
CAR_PORT = 80


def send_directions(directions):
    csv_data = io.StringIO()
    writer = csv.writer(csv_data)
    for direction in directions:
        writer.writerow(direction)
    headers = {'Content-Type': 'text/csv'}
    response = requests.post(
        f'http://{CAR_IP}:{CAR_PORT}/directions', data=csv_data.getvalue(), headers=headers)
    if response.status_code == 200:
        print('Request sent successfully')
    else:
        print(f'Failed to send request: {response.content}')


my_test_path = directions.AccelerationDS(
    2000, 10, startSpeedLeft=0, startSpeedRight=0, finalSpeedLeft=255, finalSpeedRight=253) + \
    directions.ConstantSpeedDS(500, speedLeft=255, speedRight=253) + \
    directions.ConstantSpeedDS(500, speedLeft=255, speedRight=-253) + \
    directions.ConstantSpeedDS(500, speedLeft=-255, speedRight=253) + \
    directions.ConstantSpeedDS(500, speedLeft=-255, speedRight=-253) + \
    directions.AccelerationDS(2000, 10, startSpeedLeft=-255,
                              startSpeedRight=-253, finalSpeedLeft=0, finalSpeedRight=0)


print(my_test_path)

send_directions(my_test_path)
