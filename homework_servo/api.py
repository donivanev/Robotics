import json
import uuid
from datetime import datetime
from flask import Flask, jsonify, make_response, request
import atomics

app = Flask(__name__)

events = []
next_id = atomics.atomic(width=4, atype=atomics.INT)


@app.route('/')
def hello_world():
    return 'Hello World!'


@app.route('/api/events', methods=['GET'])
def get_events():
    dt = datetime.now()
    response = make_response(
        jsonify(events),
        200,
    )
    response.headers["Content-Type"] = "application/json"
    return response


@app.route('/api/events/current', methods=['GET'])
def add_event_get_args():
    dt = datetime.now()
    args = request.args
    response = make_response(
        jsonify(
            {"timestamp": dt, "distance": args}
        ),
        200,
    )
    response.headers["Content-Type"] = "application/json"
    return response


@app.route('/api/esp32', methods=['POST'])
def post_event():
    dt = datetime.now()
    event = json.loads(request.data)
    event['id'] = next_id.fetch_inc()
    print(event)
    events.append(event)
    response = make_response(
        jsonify(
            {"status": "created", "event": event}
        ),
        201,
    )
    response.headers["Content-Type"] = "application/json"
    return response


if __name__ == '__main__':
    app.run(host='192.168.1.5', port=8080, debug=True)