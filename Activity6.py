from fastapi import FastAPI, Request
from pymongo import MongoClient
from datetime import datetime
import uvicorn

app = FastAPI()

# Connect to local MongoDB
client = MongoClient("mongodb://localhost:27017/")
db = client["esp32_db"]  # Database will auto-create when data is inserted
collection = db["dht11_data"]

# POST route: receive data from ESP32
@app.post("/esp32/post-data")
async def receive_data(request: Request):
    try:
        data = await request.json()
        temp = float(data.get("temperature"))
        hum = float(data.get("humidity"))
        device_id = data.get("device_id", "ESP32_1")

        # Data document
        doc = {
            "device_id": device_id,
            "temperature": temp,
            "humidity": hum,
            "timestamp": datetime.utcnow(),
            "warning": "High Temp!" if temp > 30 else "Normal"
        }

        result = collection.insert_one(doc)
        print(f"Received from {device_id}: {temp}°C, {hum}%")

        return {"status": "success", "inserted_id": str(result.inserted_id)}

    except Exception as e:
        print("Error:", e)
        return {"status": "error", "message": str(e)}


# GET route: retrieve last 10 entries
@app.get("/esp32/get-data")
async def get_data():
    docs = list(collection.find().sort("timestamp", -1).limit(10))
    for d in docs:
        d["_id"] = str(d["_id"])
        d["timestamp"] = d["timestamp"].isoformat()
    return {"status": "success", "data": docs}


# Run the API
if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)
