from fastapi import FastAPI, Request
import uvicorn

app = FastAPI()

@app.post("/esp32/post-data")
async def receive_data(request: Request):
    data = await request.json()
    print("Received:", data)
    return {"status": "success", "received": data}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)
