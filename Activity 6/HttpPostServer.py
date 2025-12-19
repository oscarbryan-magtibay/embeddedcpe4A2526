from fastapi import FastAPI, Request, HTTPException, BackgroundTasks
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from datetime import datetime, timezone
import uvicorn
import random
import asyncio
from contextlib import asynccontextmanager
import logging
from typing import List, Dict, Any

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Global variables
sensor_readings: List[Dict[str, Any]] = []
simulation_active = False
simulation_task = None

# In-memory storage configuration
MAX_READINGS = 1000  # Maximum readings to keep in memory

@asynccontextmanager
async def lifespan(app: FastAPI):
    """
    Lifespan context manager for startup and shutdown events
    """
    global simulation_active, simulation_task
    
    # Startup
    logger.info("🚀 Starting ESP32 DHT11 Dashboard...")
    
    # Start simulation in background
    simulation_active = True
    simulation_task = asyncio.create_task(generate_simulated_data())
    
    yield
    
    # Shutdown
    logger.info("🛑 Shutting down...")
    simulation_active = False
    if simulation_task:
        simulation_task.cancel()
        try:
            await simulation_task
        except asyncio.CancelledError:
            pass

app = FastAPI(title="ESP32 DHT11 Sensor Dashboard", lifespan=lifespan)

# Serve static files (CSS, JS)
app.mount("/static", StaticFiles(directory="static"), name="static")
templates = Jinja2Templates(directory="templates")

# Simulation parameters
SIMULATION_INTERVAL = 10  # seconds
SIMULATION_BASE_TEMP = 24.5
SIMULATION_BASE_HUMIDITY = 55.0

async def generate_simulated_data():
    """
    Background task to generate simulated DHT11 data every 10 seconds
    """
    global simulation_active, sensor_readings
    
    logger.info(f"🔧 Starting sensor simulation (every {SIMULATION_INTERVAL}s)")
    
    # Track last values for realistic progression
    last_temp = SIMULATION_BASE_TEMP
    last_humidity = SIMULATION_BASE_HUMIDITY
    
    device_id = "simulated-esp32-001"
    location = "Simulation Lab"
    
    while simulation_active:
        try:
            # Generate realistic sensor data with small variations
            # Temperature: +/- 2°C variation, trending slowly
            temp_variation = random.uniform(-0.5, 0.5)
            humidity_variation = random.uniform(-2, 2)
            
            # Add variation to last values
            temperature = last_temp + temp_variation
            humidity = last_humidity + humidity_variation
            
            # Keep values within realistic DHT11 ranges
            temperature = max(0, min(50, temperature))  # DHT11 range: 0-50°C
            humidity = max(20, min(90, humidity))      # DHT11 range: 20-90%
            
            # Calculate heat index (simplified formula)
            heat_index = calculate_heat_index(temperature, humidity)
            
            # Prepare sensor data
            sensor_data = {
                "temperature": round(temperature, 1),
                "humidity": round(humidity, 1),
                "heat_index": round(heat_index, 1),
                "device_id": device_id,
                "location": location,
                "simulated": True,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            
            # Store in memory
            store_reading(sensor_data)
            
            # Update last values for next iteration
            last_temp = temperature
            last_humidity = humidity
            
            logger.info(f"📡 Simulated data generated: {temperature}°C, {humidity}%")
            
            # Wait for next interval
            await asyncio.sleep(SIMULATION_INTERVAL)
            
        except asyncio.CancelledError:
            logger.info("🛑 Sensor simulation cancelled")
            break
        except Exception as e:
            logger.error(f"❌ Error in simulation task: {e}")
            await asyncio.sleep(SIMULATION_INTERVAL)

def store_reading(data: Dict[str, Any]):
    """Store sensor reading in memory"""
    global sensor_readings
    
    # Add to beginning of list (newest first)
    sensor_readings.insert(0, data)
    
    # Keep only the most recent readings
    if len(sensor_readings) > MAX_READINGS:
        sensor_readings = sensor_readings[:MAX_READINGS]

def calculate_heat_index(temperature: float, humidity: float) -> float:
    """
    Calculate heat index (simplified formula for moderate temperatures)
    Using the Rothfusz regression for temperatures above 27°C
    """
    # Simple approximation for demo purposes
    if temperature < 27:
        return temperature  # No significant heat index effect below 27°C
    
    # Simplified heat index calculation
    HI = -8.78469475556
    HI += 1.61139411 * temperature
    HI += 2.33854883889 * humidity
    HI += -0.14611605 * temperature * humidity
    HI += -0.012308094 * temperature * temperature
    HI += -0.0164248277778 * humidity * humidity
    HI += 0.002211732 * temperature * temperature * humidity
    HI += 0.00072546 * temperature * humidity * humidity
    HI += -0.000003582 * temperature * temperature * humidity * humidity
    
    return max(temperature, round(HI, 1))

@app.get("/", response_class=HTMLResponse)
async def dashboard(request: Request):
    """Main dashboard page"""
    return templates.TemplateResponse("dashboard.html", {"request": request})

@app.post("/esp32/post-data")
async def receive_data(request: Request):
    """Receive data from ESP32 or other clients"""
    try:
        data = await request.json()
        logger.info("📨 Received data from ESP32: %s", data)
        
        # Add timestamp if not provided
        if "timestamp" not in data:
            data["timestamp"] = datetime.now(timezone.utc).isoformat()
        
        # Mark as real data (not simulated)
        if "simulated" not in data:
            data["simulated"] = False
        
        # Store in memory
        store_reading(data)
        
        logger.info(f"✅ Real data stored: {data.get('temperature', 'N/A')}°C, {data.get('humidity', 'N/A')}%")
        
        return JSONResponse(
            status_code=200,
            content={
                "status": "success", 
                "message": "Data received and stored",
                "simulated": data.get("simulated", False)
            }
        )
        
    except Exception as e:
        logger.error(f"❌ Error receiving data: {e}")
        return JSONResponse(
            status_code=500,
            content={"status": "error", "message": str(e)}
        )

@app.get("/api/sensor-readings")
async def get_sensor_readings(limit: int = 20, simulated: bool = None):
    """API endpoint for sensor readings"""
    try:
        # Filter readings if requested
        if simulated is None:
            filtered_readings = sensor_readings[:limit]
        else:
            filtered_readings = [
                reading for reading in sensor_readings 
                if reading.get("simulated") == simulated
            ][:limit]
        
        return JSONResponse(
            status_code=200,
            content={
                "total": len(sensor_readings),
                "filtered": len(filtered_readings),
                "readings": filtered_readings
            }
        )
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/latest-reading")
async def get_latest_reading(include_simulated: bool = True):
    """Get the most recent sensor reading"""
    try:
        if not sensor_readings:
            return JSONResponse(
                status_code=200,
                content={}
            )
        
        if include_simulated:
            latest = sensor_readings[0] if sensor_readings else {}
        else:
            # Find first non-simulated reading
            latest = next(
                (reading for reading in sensor_readings if not reading.get("simulated", False)), 
                {}
            )
        
        return JSONResponse(
            status_code=200,
            content=latest
        )
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/stats")
async def get_stats(include_simulated: bool = True):
    """Get sensor statistics"""
    try:
        if not sensor_readings:
            return JSONResponse(
                status_code=200,
                content={
                    "total_readings": 0,
                    "current": None,
                    "averages": {},
                    "simulation_active": simulation_active
                }
            )
        
        # Filter readings if needed
        if include_simulated:
            readings_to_analyze = sensor_readings[:100]  # Last 100 readings
        else:
            readings_to_analyze = [
                reading for reading in sensor_readings[:100] 
                if not reading.get("simulated", False)
            ]
        
        # Get latest reading for current values
        latest = sensor_readings[0] if sensor_readings else {}
        
        if not include_simulated:
            # Find latest non-simulated reading
            latest = next(
                (reading for reading in sensor_readings if not reading.get("simulated", False)), 
                {}
            )
        
        # Calculate statistics
        if readings_to_analyze:
            temperatures = [r.get("temperature", 0) for r in readings_to_analyze if "temperature" in r]
            humidities = [r.get("humidity", 0) for r in readings_to_analyze if "humidity" in r]
            
            if temperatures and humidities:
                stats = {
                    "avg_temperature": round(sum(temperatures) / len(temperatures), 1),
                    "avg_humidity": round(sum(humidities) / len(humidities), 1),
                    "max_temperature": round(max(temperatures), 1),
                    "min_temperature": round(min(temperatures), 1),
                    "max_humidity": round(max(humidities), 1),
                    "min_humidity": round(min(humidities), 1),
                    "count": len(readings_to_analyze)
                }
            else:
                stats = {}
        else:
            stats = {}
        
        response_data = {
            "total_readings": len(sensor_readings),
            "current": latest,
            "averages": stats,
            "simulation_active": simulation_active,
            "include_simulated": include_simulated
        }
        
        return JSONResponse(
            status_code=200,
            content=response_data
        )
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/simulation/control")
async def control_simulation(action: str = "toggle"):
    """Control the simulation (start/stop/toggle)"""
    global simulation_active, simulation_task
    
    if action == "start":
        if not simulation_active:
            simulation_active = True
            simulation_task = asyncio.create_task(generate_simulated_data())
            return {"status": "started", "message": "Simulation started"}
        else:
            return {"status": "already_running", "message": "Simulation already running"}
    
    elif action == "stop":
        if simulation_active:
            simulation_active = False
            if simulation_task:
                simulation_task.cancel()
            return {"status": "stopped", "message": "Simulation stopped"}
        else:
            return {"status": "already_stopped", "message": "Simulation already stopped"}
    
    elif action == "toggle":
        if simulation_active:
            simulation_active = False
            if simulation_task:
                simulation_task.cancel()
            return {"status": "stopped", "message": "Simulation stopped"}
        else:
            simulation_active = True
            simulation_task = asyncio.create_task(generate_simulated_data())
            return {"status": "started", "message": "Simulation started"}
    
    elif action == "status":
        return {
            "status": "running" if simulation_active else "stopped",
            "interval_seconds": SIMULATION_INTERVAL,
            "base_temperature": SIMULATION_BASE_TEMP,
            "base_humidity": SIMULATION_BASE_HUMIDITY,
            "stored_readings": len(sensor_readings),
            "max_readings": MAX_READINGS
        }
    
    else:
        raise HTTPException(status_code=400, detail="Invalid action. Use 'start', 'stop', 'toggle', or 'status'")

@app.post("/api/simulation/send-test")
async def send_test_reading(
    temperature: float = None,
    humidity: float = None
):
    """Manually send a test reading"""
    try:
        # Use provided values or generate random ones
        if temperature is None:
            temperature = round(random.uniform(20, 30), 1)
        if humidity is None:
            humidity = round(random.uniform(40, 70), 1)
        
        heat_index = calculate_heat_index(temperature, humidity)
        
        sensor_data = {
            "temperature": temperature,
            "humidity": humidity,
            "heat_index": round(heat_index, 1),
            "device_id": "manual-test",
            "location": "Manual Test",
            "simulated": True,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        # Store in memory
        store_reading(sensor_data)
        
        return {
            "status": "success",
            "message": "Test reading sent",
            "data": {
                "temperature": temperature,
                "humidity": humidity,
                "heat_index": heat_index
            }
        }
        
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/reset")
async def reset_data():
    """Reset all stored data (for testing)"""
    global sensor_readings
    previous_count = len(sensor_readings)
    sensor_readings = []
    
    return {
        "status": "success",
        "message": "All data cleared",
        "previous_readings": previous_count,
        "current_readings": 0
    }

@app.get("/api/system-status")
async def system_status():
    """Get system status information"""
    return {
        "status": "running",
        "simulation": {
            "active": simulation_active,
            "interval_seconds": SIMULATION_INTERVAL
        },
        "storage": {
            "current_readings": len(sensor_readings),
            "max_readings": MAX_READINGS,
            "memory_usage": f"{len(sensor_readings) * 500 / 1024:.1f} KB (approx)"
        },
        "api_endpoints": {
            "dashboard": "/",
            "post_data": "/esp32/post-data",
            "readings": "/api/sensor-readings",
            "stats": "/api/stats",
            "simulation_control": "/api/simulation/control",
            "send_test": "/api/simulation/send-test",
            "reset": "/api/reset"
        }
    }

if __name__ == "__main__":
    print("🚀 Starting ESP32 DHT11 Dashboard with In-Memory Storage...")
    print("📊 Dashboard: http://localhost:8000")
    print("📚 API Docs: http://localhost:8000/docs")
    print("🔧 Simulation: Auto-generates data every 10 seconds")
    print("💾 Storage: In-memory (no database required)")
    print("")
    print("📋 Available API endpoints:")
    print("   GET  /                         - Dashboard")
    print("   POST /esp32/post-data          - Post sensor data")
    print("   GET  /api/sensor-readings      - Get recent readings")
    print("   GET  /api/latest-reading       - Get latest reading")
    print("   GET  /api/stats                - Get statistics")
    print("   POST /api/simulation/control   - Control simulation")
    print("   POST /api/simulation/send-test - Send test reading")
    print("   GET  /api/reset                - Clear all data")
    print("   GET  /api/system-status        - System info")
    
    # Run the server
    uvicorn.run(app, host="192.168.254.110", port=8000, reload=True)