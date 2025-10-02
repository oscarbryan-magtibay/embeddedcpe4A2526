from fastapi import FastAPI
from pydantic import BaseModel
from pymongo import MongoClient
from datetime import datetime

app = FastAPI()

# MongoDB setup
client = MongoClient("mongodb://localhost:27017")
db = client["ESP32andDHT11"]
collection = db["Readings"]

# Pydantic model for request body validation
class DHTData(BaseModel):
    temperature: float
    humidity: float

# POST endpoint
@app.post("/esp32/post-data")
async def post_data(data: DHTData):
    record = data.dict()
    record["timestamp"] = datetime.utcnow()
    result = collection.insert_one(record)
    return {"status": "success", "inserted_id": str(result.inserted_id), "data": record}

# GET endpoint to retrieve last 10 readings
@app.get("/esp32/get-data")
async def get_data():
    readings = list(collection.find().sort("_id", -1).limit(10))
    for r in readings:
        r["_id"] = str(r["_id"])
    return readings

# Optional test endpoint to check connectivity
@app.get("/test")
def test():
    return {"status": "server reachable"}
