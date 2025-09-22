from fastapi import FastAPI, Request
from pymongo import MongoClient

app = FastAPI()

client = MongoClient("mongodb://localhost:27017/")
db = client["DHT11_db"]         
collection = db["readings"]    

@app.post("/esp32/post-data")
async def receive_data(request: Request):
    data = await request.json()
    print("Received:", data)
    
    result = collection.insert_one(data)
    print("Inserted ID:", result.inserted_id)
    
    return {
        "status": "success",
        "received": data,
        "inserted_id": str(result.inserted_id)
    }
