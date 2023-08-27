import React, { useEffect, useState, useRef } from 'react';
import './App.css';
import apiClient from './api-client';

interface EventData {
  _id: string,
  time: number,
  distance: number
}

function App() {
  const [distanceLeft, setDistanceLeft] = useState<EventData>();
  const [errors, setErrors] = useState<any>();
  const interval = useRef<NodeJS.Timer>();

  useEffect(() => {
    interval.current = setInterval(() =>
      apiClient.getLatestData().then(res => setDistanceLeft(res))
      .catch(err => {
        console.log(err)
        setErrors(err)
      })
    , 500)
    return () => {
      clearInterval(interval.current)
    }
  }, [])

  return (
    <div className="App">
      <header className="App-header">
        <h2>Distance - left sensor: {distanceLeft?.distance} in {distanceLeft?.time}</h2>
        {errors && <h3>Error: {errors}</h3>}
      </header>
    </div>
  );
}

export default App;
