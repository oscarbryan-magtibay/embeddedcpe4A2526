# main.py
from fastapi import FastAPI
from pydantic import BaseModel
from motor.motor_asyncio import AsyncIOMotorClient
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse, JSONResponse
from datetime import datetime
import uvicorn
import os

app = FastAPI()

# --- MongoDB settings ---
MONGO_URI = "mongodb://localhost:27017"   # default for local MongoDB
MONGO_DB = "dht11_monitor"
MONGO_COLL = "readings"

mongo_client = AsyncIOMotorClient(MONGO_URI)
db = mongo_client[MONGO_DB]
coll = db[MONGO_COLL]

# --- static files (served at / and /static) ---
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
STATIC_DIR = os.path.join(BASE_DIR, "static")
app.mount("/static", StaticFiles(directory=STATIC_DIR), name="static")

# --- Pydantic model for incoming ESP32 data ---
class DHTReading(BaseModel):
    temperature: float
    humidity: float
    device: str = "esp32"

# --- ESP32 posting endpoint ---
@app.post("/esp32/post-data")
async def receive_data(reading: DHTReading):
    doc = reading.dict()
    doc["timestamp"] = datetime.utcnow()  # store as datetime for queries
    inserted = await coll.insert_one(doc)
    return {"status": "success", "id": str(inserted.inserted_id)}

# --- Old endpoint (still available) ---
@app.get("/data")
async def get_data(limit: int = 100):
    cursor = coll.find().sort("timestamp", -1).limit(limit)
    docs = []
    async for d in cursor:
        docs.append({
            "id": str(d.get("_id")),
            "temperature": d.get("temperature"),
            "humidity": d.get("humidity"),
            "device": d.get("device"),
            "timestamp": d.get("timestamp").isoformat() if d.get("timestamp") else None
        })
    return JSONResponse(docs)

# --- New endpoint: /api/sensor-readings (for frontend JS) ---
@app.get("/api/sensor-readings")
async def sensor_readings(limit: int = 10):
    cursor = coll.find().sort("timestamp", -1).limit(limit)
    docs = []
    async for d in cursor:
        docs.append({
            "id": str(d.get("_id")),
            "temperature": d.get("temperature"),
            "humidity": d.get("humidity"),
            "device": d.get("device"),
            "timestamp": d.get("timestamp").isoformat() if d.get("timestamp") else None
        })
    return {"readings": docs}

# --- New endpoint: /api/stats (for summary data) ---
@app.get("/api/stats")
async def stats():
    count = await coll.count_documents({})
    latest = await coll.find().sort("timestamp", -1).to_list(1)
    latest_doc = latest[0] if latest else None

    return {
        "total_records": count,
        "latest": {
            "temperature": latest_doc.get("temperature") if latest_doc else None,
            "humidity": latest_doc.get("humidity") if latest_doc else None,
            "device": latest_doc.get("device") if latest_doc else None,
            "timestamp": latest_doc.get("timestamp").isoformat() if latest_doc and latest_doc.get("timestamp") else None
        } if latest_doc else None
    }

# --- Root serves index.html ---
@app.get("/")
async def index():
    return FileResponse(os.path.join(STATIC_DIR, "index.html"))

if __name__ == "__main__":
    uvicorn.run("main:app", host="0.0.0.0", port=8000, reload=True)
