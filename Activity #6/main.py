from fastapi import FastAPI, Request
import uvicorn
from pymongo import MongoClient
from datetime import datetime

app = FastAPI()

client = MongoClient("mongodb://localhost:27017/")

db = client["dbtempESP32"]          
collection = db["dht11_data"]     

@app.post("/esp32/post-data")
async def receive_data(request: Request):
    data = await request.json()
    try:
        data["Temperature"] = float(data.get("Temperature"))
        data["Humidity"] = float(data.get("Humidity"))
    except Exception:
        pass

    data["timestamp"] = datetime.utcnow()

    result = collection.insert_one(data)

    response = {
        "status": "success",
        "inserted_id": str(result.inserted_id),
        "received": {
            "Temperature": data.get("Temperature"),
            "Humidity": data.get("Humidity"),
            "timestamp": data["timestamp"].isoformat() + "Z"
        }
    }
    
    print("Received and stored:", data)
    return response

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)