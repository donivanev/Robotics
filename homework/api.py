import json
from datetime import datetime
from flask import Flask, jsonify, make_response, request
from flask_cors import CORS
from pymongo import MongoClient
from bson.json_util import dumps, ObjectId

app = Flask(__name__)
CORS(app)

client = MongoClient('localhost', 27017)
db = client.robot
events_db = db.events

db2 = client.slider
slider_db = db.sliderValues

servoAngles = [90, 90]

class JSONEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, ObjectId):
            return str(o)
        return json.JSONEncoder.default(self, o)

encoder = JSONEncoder()

@app.route('/')
def hello_world():
    return 'Hello World!'


@app.route('/api/events', methods=['GET'])
def get_events():
    events = list(events_db.find())
    response = make_response(
        encoder.encode(events),
        200,
    )
    response.headers["Content-Type"] = "application/json"
    return response


@app.route('/api/events/current', methods=['GET'])
def add_event_get_args():
    dt = datetime.now()
    latest_event = events_db.find().skip(events_db.estimated_document_count() - 1).next()
    response = make_response(
        encoder.encode(latest_event),
        200,
    )
    response.headers["Content-Type"] = "application/json"
    return response

@app.route('/api/events/slider', methods=['GET'])
def get_slider_value():
    dt = datetime.now()
    #latest_event = slider_db.find().skip(slider_db.estimated_document_count() - 1).next()
    servoJson = '{"leftAngle":' + str(servoAngles[0]) + '", rightAngle":' + str(servoAngles[1]) + '}'
    response = make_response(
        encoder.encode(servoJson),
        200,
    )
    response.headers["Content-Type"] = "application/json"
    return response

@app.route('/api/esp32', methods=['POST'])
def post_event():
    dt = datetime.now()
    event = json.loads(request.data)
    events_db.insert_one(event)
    encoded = encoder.encode(event)
    print(encoded)
    response = make_response(
        encoded,
        201,
    )
    response.headers["Content-Type"] = "application/json"
    return response

@app.route('/api/esp32/slider', methods=['POST'])
def post_slider_value_event():
    dt = datetime.now()
    event = json.loads(request.data)
    print(event)
    slider_db.insert_one(event)
    encoded = encoder.encode(event)
    servoAngles[event["servoNumber"]] = event["angle"]
    print(encoded) 
    response = make_response(
        encoded,
        201,
    )
    response.headers["Content-Type"] = "application/json"
    return response


if __name__ == '__main__':
    app.run(host='192.168.1.102', port=8080, debug=True)