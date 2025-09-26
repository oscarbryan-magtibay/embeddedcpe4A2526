from fastapi import FastAPI, Request
from pymongo import MongoClient
import uvicorn
import certifi
from datetime import datetime

app = FastAPI()

# MongoDB Atlas connection
client = MongoClient(
    "mongodb+srv://abigailabienbitay:201012@cluster0.v0k79fs.mongodb.net/?retryWrites=true&w=majority",
    tlsCAFile=certifi.where()
)
db = client["Telemetry"]
collection = db["telemetryData"]

# Helper to convert ObjectId and datetime to string
def serialize_doc(doc):
    doc["_id"] = str(doc["_id"])
    if "timestamp" in doc:
        doc["timestamp"] = doc["timestamp"].isoformat()
    return doc

@app.post("/esp32/post-data")
async def receive_data(request: Request):
    try:
        data = await request.json()
        print("📩 Received:", data)

        # Add timestamp
        data["timestamp"] = datetime.utcnow()

        # Insert into MongoDB
        result = collection.insert_one(data)

        # Return a JSON-serializable response
        return {
            "status": "success",
            "inserted_id": str(result.inserted_id),
            "received": serialize_doc(data)
        }
    except Exception as e:
        print("❌ Error:", str(e))
        return {"status": "error", "message": str(e)}

@app.get("/esp32/get-data")
async def get_data():
    docs = list(collection.find().sort("timestamp", -1).limit(10))
    return {"status": "success", "data": [serialize_doc(doc) for doc in docs]}

if __name__ == "__main__":
    uvicorn.run("server:app", host="0.0.0.0", port=8000, reload=True)
