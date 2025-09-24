from fastapi import FastAPI, Request
import uvicorn
from pymongo import MongoClient
from datetime import datetime

app = FastAPI()

client = MongoClient("mongodb://localhost:27017/")
db = client["esp32_db"]
collection = db["dht11_data"]

@app.post("/esp32/post-data")
async def receive_data(request: Request):
    try:
        data = await request.json()  
        temp = float(data.get("Temperature"))
        hum = float(data.get("Humidity"))

        doc = {
            "Temperature": temp,
            "Humidity": hum,
            "timestamp": datetime.utcnow()
        }
        result = collection.insert_one(doc)

        print(f"📡 Received -> Temp: {temp}, Humidity: {hum}")

        return {
            "status": "success",
            "inserted_id": str(result.inserted_id),
            "data": doc
        }

    except Exception as e:
        print(" Error:", e)
        return {"status": "error", "message": str(e)}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)
