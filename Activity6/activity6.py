from fastapi import FastAPI 
from pydantic import BaseModel
from pymongo import MongoClient
from datetime import datetime
from bson import ObjectId  

app = FastAPI()

client = MongoClient("mongodb://localhost:27017")
db = client["DHT11_"]
collection = db["dht11"]

class DHTData(BaseModel):
    temperature: float
    humidity: float

@app.post("/esp32/post-data")
async def post_data(data: DHTData):
    record = data.dict()
    record["timestamp"] = datetime.utcnow()

    result = collection.insert_one(record)

    # Prepare clean response
    response = {
        "status": "success",
        "inserted_id": str(result.inserted_id),
        "data": {
            "temperature": record["temperature"],
            "humidity": record["humidity"],
            "timestamp": record["timestamp"].isoformat()
        }
    }
    return response

@app.get("/esp32/get-data")
async def get_data():
    readings = list(collection.find().sort("_id", -1).limit(10))
    formatted = []
    for r in readings:
        formatted.append({
            "_id": str(r["_id"]),
            "temperature": r["temperature"],
            "humidity": r["humidity"],
            "timestamp": r["timestamp"].isoformat() if "timestamp" in r else None
        })
    return formatted

@app.get("/test")
def test():
    return {"status": "server reachable"}
