from fastapi import FastAPI, Request, HTTPException
import uvicorn
from pymongo import MongoClient
from datetime import datetime

app = FastAPI()

# MongoDB connection
client = MongoClient("mongodb://localhost:27017/")  
db = client["DBESP32Temp"]            
collection = db["dht11_datatemp"]  # Collection for storing sensor data

@app.post("/esp32/post-data")
async def receive_data(request: Request):
    try:
        # Parse incoming JSON data
        data = await request.json()

        # Validate and convert data fields to float
        temperature = data.get("Temperature")
        humidity = data.get("Humidity")
        
        if temperature is None or humidity is None:
            raise HTTPException(status_code=400, detail="Temperature and Humidity are required.")
        
        data["Temperature"] = float(temperature)
        data["Humidity"] = float(humidity)
        
        # Add timestamp in UTC
        data["timestamp"] = datetime.utcnow()

        # Insert data into MongoDB collection
        result = collection.insert_one(data)

        # Prepare and return the response
        response = {
            "status": "success",
            "inserted_id": str(result.inserted_id),
            "received": {
                "Temperature": data["Temperature"],
                "Humidity": data["Humidity"],
                "timestamp": data["timestamp"].isoformat() + "Z"
            }
        }

        print("Received and stored:", data)
        return response

    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)
