from fastapi import FastAPI, Request, HTTPException
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from pymongo import MongoClient
from datetime import datetime, timezone
import uvicorn
from bson import ObjectId
import json

app = FastAPI(title="ESP32 DHT11 Sensor Dashboard")

# Serve static files (CSS, JS)
app.mount("/static", StaticFiles(directory="static"), name="static")
templates = Jinja2Templates(directory="templates")

# MongoDB configuration
try:
    client = MongoClient("mongodb://localhost:27017/")
    db = client["sensor_data"]
    collection = db["dht11_readings"]
    print("✅ MongoDB connected successfully!")
except Exception as e:
    print(f"❌ MongoDB connection failed: {e}")
    client = None

# Custom JSON encoder to handle MongoDB types
def serialize_document(doc):
    """Convert MongoDB document to JSON-serializable dict"""
    if doc is None:
        return None
    
    serialized = {}
    for key, value in doc.items():
        if isinstance(value, ObjectId):
            serialized[key] = str(value)
        elif isinstance(value, datetime):
            serialized[key] = value.isoformat()
        else:
            serialized[key] = value
    return serialized

@app.get("/", response_class=HTMLResponse)
async def dashboard(request: Request):
    """Main dashboard page"""
    return templates.TemplateResponse("dashboard.html", {"request": request})

@app.post("/esp32/post-data")
async def receive_data(request: Request):
    try:
        data = await request.json()
        print("📨 Received data:", data)
        
        if client is None:
            return JSONResponse(
                status_code=500,
                content={"status": "error", "message": "Database not available"}
            )
        
        # Add timestamp
        data["timestamp"] = datetime.now(timezone.utc)
        
        # Insert into MongoDB
        result = collection.insert_one(data)
        print(f"✅ Data stored with ID: {result.inserted_id}")
        
        return JSONResponse(
            status_code=200,
            content={
                "status": "success", 
                "message": "Data received and stored",
                "id": str(result.inserted_id)
            }
        )
        
    except Exception as e:
        print(f"❌ Error: {e}")
        return JSONResponse(
            status_code=500,
            content={"status": "error", "message": str(e)}
        )

@app.get("/api/sensor-readings")
async def get_sensor_readings(limit: int = 20):
    """API endpoint for sensor readings"""
    if client is None:
        raise HTTPException(status_code=500, detail="Database not connected")
    
    try:
        readings = list(collection.find().sort("timestamp", -1).limit(limit))
        
        # Convert for JSON response using our custom serializer
        serialized_readings = [serialize_document(reading) for reading in readings]
        
        return JSONResponse(
            status_code=200,
            content={
                "total": len(serialized_readings),
                "readings": serialized_readings
            }
        )
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/latest-reading")
async def get_latest_reading():
    """Get the most recent sensor reading"""
    if client is None:
        raise HTTPException(status_code=500, detail="Database not connected")
    
    try:
        latest = collection.find_one(sort=[("timestamp", -1)])
        serialized_latest = serialize_document(latest)
        
        return JSONResponse(
            status_code=200,
            content=serialized_latest
        )
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/stats")
async def get_stats():
    """Get sensor statistics"""
    if client is None:
        raise HTTPException(status_code=500, detail="Database not connected")
    
    try:
        total_readings = collection.count_documents({})
        
        # Get latest reading for current values
        latest = collection.find_one(sort=[("timestamp", -1)])
        
        # Get averages
        pipeline = [
            {"$sort": {"timestamp": -1}},
            {"$limit": 100},  # Last 100 readings
            {"$group": {
                "_id": None,
                "avg_temperature": {"$avg": "$temperature"},
                "avg_humidity": {"$avg": "$humidity"},
                "max_temperature": {"$max": "$temperature"},
                "min_temperature": {"$min": "$temperature"},
                "max_humidity": {"$max": "$humidity"},
                "min_humidity": {"$min": "$humidity"}
            }}
        ]
        
        stats_result = list(collection.aggregate(pipeline))
        stats = stats_result[0] if stats_result else {}
        
        # Serialize the response
        response_data = {
            "total_readings": total_readings,
            "current": serialize_document(latest),
            "averages": serialize_document(stats)  # This handles BSON Decimal128 types
        }
        
        return JSONResponse(
            status_code=200,
            content=response_data
        )
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

if __name__ == "__main__":
    print("🚀 Starting ESP32 DHT11 Dashboard...")
    print("📊 Dashboard: http://localhost:8000")
    print("📚 API Docs: http://localhost:8000/docs")
    uvicorn.run(app, host="0.0.0.0", port=8000, reload=True)