import { useEffect, useState, useRef } from 'react'
import { Slider } from '@mui/material';
import './App.css';

function App() {

  const [speed, setSpeed] = useState(0);
  const [forward, setForward] = useState(false);
  const [backward, setBackward] = useState(false);
  const [stop, setStop] = useState(false);
  const [angle, setAngle] = useState(0);
  const [servoNumber, setServoNumber] = useState(0);
  const interval = useRef(1);

  const moveForward = (servoNumber) => {
    fetch('http://192.168.1.102:8080/api/esp32', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            servo: servoNumber,
            forward: true,
            backward: false,
            stop: false
        })
    })
  }

  const moveBackward = (servoNumber) => {
    fetch('http://192.168.1.102:8080/api/esp32', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          servo: servoNumber,
          forward: false,
          backward: true,
          stop: false
        })
    })
  }

  const stopMotion = (servoNumber) => {
    fetch('http://192.168.1.102:8080/api/esp32', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          servo: servoNumber,
          forward: false,
          backward: false,
          stop: true
        })
    })
  }

  useEffect(() => {
    interval.current = setInterval(() =>
    fetch('http://192.168.1.102:8080/api/esp32/slider', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                servoNumber,
                angle
            })
        })
    , 2000)
    return () => {
      clearInterval(interval.current)
    }
  }, [angle, servoNumber])

  return (
    <div className="main">
      <div className="column">
        <label htmlFor="motorL">Motor L</label>
        <input type="text" value={speed} />
        <Slider defaultValue={0} aria-label="Default" valueLabelDisplay="auto" style={{width: "200px"}} 
                onChange={(event, value) => {setAngle(value); setServoNumber(0)}}/>
        <button onClick={() => moveForward(1)}>Forward</button>
        <button onClick={() => moveBackward(1)}>Backward</button>
        <button onClick={() => stopMotion(1)}>Stop</button>
      </div>
      <div className="column">
        <label htmlFor="motorR">Motor R</label>
        <input type="text" value={speed} />
        <Slider defaultValue={0} aria-label="Default" valueLabelDisplay="auto" style={{width: "200px"}}
                onChange={(event, value) => {setAngle(value); setServoNumber(1)}}/>
        <button onClick={() => moveForward(2)}>Forward</button>
        <button onClick={() => moveBackward(2)}>Backward</button>
        <button onClick={() => stopMotion(2)}>Stop</button>
      </div>
    </div>
  );
}

export default App;
