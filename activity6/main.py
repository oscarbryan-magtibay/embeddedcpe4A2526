from fastapi import FastAPI, Request
from pydantic import BaseModel
from datetime import datetime
from pymongo import MongoClient

app = FastAPI()

# MongoDB setup
client = MongoClient("mongodb+srv://2220194_db_user:ubian2022@cluster0.yjezdts.mongodb.net/")
db = client["Temperature"]
collection = db["temperaturedht11"]

# Pydantic model for validation
class SensorData(BaseModel):
    temperature: float
    humidity: float

@app.post("/data")
async def receive_data(data: SensorData):
    # Log to console
    print(f"Received: Temp={data.temperature}, Humidity={data.humidity}")

    # Save to MongoDB with timestamp
    collection.insert_one({
        "temperature": data.temperature,
        "humidity": data.humidity,
        "timestamp": datetime.utcnow()
    })

    return {"status": "success"}