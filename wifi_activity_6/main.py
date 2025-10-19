from fastapi import FastAPI, Request
from pymongo import MongoClient
from bson import ObjectId
from datetime import datetime

app = FastAPI()

# Connect to MongoDB
client = MongoClient("mongodb://localhost:27017/")  # your MongoDB URI
db = client["Telemetry"]  # database name
collection = db["telemetryData"]  # collection name

# Helper to convert ObjectId to string
def serialize_doc(doc):
    doc["_id"] = str(doc["_id"])
    return doc

@app.post("/esp32/post-data")
async def receive_data(request: Request):
    data = await request.json()
    
    # Add timestamp
    data["timestamp"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    print("Received:", data)

    # Insert into MongoDB
    result = collection.insert_one(data)
    print("Data inserted into MongoDB!")

    # Return the inserted document
    inserted_doc = collection.find_one({"_id": result.inserted_id})
    return {"status": "success", "received": serialize_doc(inserted_doc)}
