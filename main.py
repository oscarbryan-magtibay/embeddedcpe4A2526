from fastapi import FastAPI, Request
import uvicorn
from pymongo import MongoClient

# Connect to local MongoDB
client = MongoClient("mongodb://localhost:27017/")
db = client["room_environment"]         # Database name
collection = db["dht11_data"]           # Collection name

app = FastAPI()

@app.post("/esp32/post-data")
async def receive_data(request: Request):
    data = await request.json()
    print("Received:", data)

    # Insert data into MongoDB
    result = collection.insert_one(data)
    print(f"Inserted with ID: {result.inserted_id}")

    return {"status": "success", "received": data}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)

# Run with:
# uvicorn main:app --host 0.0.0.0 --port 8000
