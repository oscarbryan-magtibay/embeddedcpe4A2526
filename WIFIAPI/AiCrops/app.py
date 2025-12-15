# app.py - COMPLETE CODE WITH DEBUG PRINTING
from flask import Flask, request, jsonify, render_template
from flask_cors import CORS
import sqlite3
from datetime import datetime
import json
import os

app = Flask(__name__)
CORS(app, resources={
    r"/api/*": {
        "origins": "*",
        "methods": ["GET", "POST", "OPTIONS"],
        "allow_headers": ["Content-Type"]
    }
})

# Handle OPTIONS requests
@app.route('/api/sensor-data', methods=['OPTIONS'])
def handle_options():
    return '', 200

# Initialize database with print statements
def init_db():
    print("🛠️ Initializing database...")
    
    # Check if database file exists
    db_exists = os.path.exists('crop_data.db')
    if db_exists:
        print("📁 Existing database found")
    else:
        print("📁 Creating new database")
    
    conn = sqlite3.connect('crop_data.db')
    c = conn.cursor()
    
    # Check current table structure
    c.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='sensor_data'")
    table_exists = c.fetchone() is not None
    
    if table_exists:
        print("📊 Table 'sensor_data' exists, checking columns...")
        c.execute("PRAGMA table_info(sensor_data)")
        columns = c.fetchall()
        print(f"📋 Current columns: {[col[1] for col in columns]}")
    else:
        print("📊 Table 'sensor_data' does not exist, creating...")
    
    # Create table if it doesn't exist
    c.execute('''CREATE TABLE IF NOT EXISTS sensor_data
                 (id INTEGER PRIMARY KEY AUTOINCREMENT,
                  timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
                  soil_moisture REAL,
                  soil_moisture_percent REAL,
                  temperature REAL,
                  humidity REAL,
                  stress_level TEXT,
                  analysis TEXT,
                  recommendations TEXT)''')
    
    # Verify table was created
    c.execute("PRAGMA table_info(sensor_data)")
    final_columns = c.fetchall()
    print(f"✅ Final table structure: {[col[1] for col in final_columns]}")
    
    conn.commit()
    conn.close()
    print("✅ Database initialization complete\n")

# Initialize the database
init_db()

def analyze_crop_data(soil_moisture, soil_moisture_percent, temperature, humidity):
    """Rule-based crop analysis without AI"""
    
    print(f"🔍 Analyzing crop data: Soil={soil_moisture_percent}%, Temp={temperature}°C, Humidity={humidity}%")
    
    # Determine stress level
    stress_score = 0
    issues = []
    
    # Soil moisture analysis
    if soil_moisture_percent < 20:
        stress_score += 3
        issues.append("Extreme drought")
        print("⚠️ Extreme drought detected")
    elif soil_moisture_percent < 40:
        stress_score += 2
        issues.append("Low soil moisture")
        print("⚠️ Low soil moisture detected")
    elif soil_moisture_percent > 80:
        stress_score += 1
        issues.append("Potential waterlogging")
        print("⚠️ Potential waterlogging detected")
    else:
        print("✅ Soil moisture is optimal")
    
    # Temperature analysis
    if temperature > 35:
        stress_score += 2
        issues.append("High temperature stress")
        print("🌡️ High temperature stress detected")
    elif temperature < 10:
        stress_score += 1
        issues.append("Low temperature stress")
        print("🌡️ Low temperature stress detected")
    else:
        print("✅ Temperature is optimal")
    
    # Humidity analysis
    if humidity < 30:
        stress_score += 1
        issues.append("Low humidity")
        print("💨 Low humidity detected")
    elif humidity > 80:
        stress_score += 1
        issues.append("High humidity risk")
        print("💨 High humidity risk detected")
    else:
        print("✅ Humidity is optimal")
    
    # Determine stress level
    if stress_score >= 3:
        stress_level = "Critical"
    elif stress_score == 2:
        stress_level = "High"
    elif stress_score == 1:
        stress_level = "Medium"
    else:
        stress_level = "Low"
    
    print(f"📈 Stress Score: {stress_score} -> Level: {stress_level}")
    
    # Generate analysis
    issues_text = ', '.join(issues) if issues else 'No significant issues detected'
    
    analysis = f"""
🌱 CROP HEALTH ANALYSIS:
------------------------
Stress Level: {stress_level}
Soil Moisture: {soil_moisture_percent}% {'⚠️' if soil_moisture_percent < 40 or soil_moisture_percent > 80 else '✅'}
Temperature: {temperature}°C {'⚠️' if temperature > 35 or temperature < 10 else '✅'}
Humidity: {humidity}% {'⚠️' if humidity < 30 or humidity > 80 else '✅'}

ISSUES DETECTED:
{issues_text}

PLANT PHYSIOLOGY IMPACT:
- Photosynthesis efficiency: {'Reduced' if stress_score > 0 else 'Optimal'}
- Water use efficiency: {'Poor' if any('drought' in issue.lower() or 'moisture' in issue.lower() for issue in issues) else 'Good'}
- Growth rate: {'Slowed' if stress_score > 1 else 'Normal'}
    """
    
    # Generate recommendations
    recommendations = []
    
    # Check each issue individually
    for issue in issues:
        issue_lower = issue.lower()
        if 'drought' in issue_lower or 'moisture' in issue_lower:
            if not any('irrigation' in rec for rec in recommendations):
                recommendations.append("🚰 Immediate irrigation required")
                recommendations.append("💧 Implement drip irrigation for efficiency")
                recommendations.append("🌿 Apply organic mulch to retain moisture")
        
        if 'temperature' in issue_lower:
            if not any('temperature' in rec.lower() for rec in recommendations):
                recommendations.append("☀️ Use shade nets during peak hours")
                recommendations.append("🌊 Water in early morning or late evening")
                recommendations.append("🌳 Plant windbreaks if available")
        
        if 'humidity' in issue_lower:
            if not any('humidity' in rec.lower() for rec in recommendations):
                recommendations.append("💨 Improve air circulation")
                if 'low' in issue_lower:
                    recommendations.append("🌫️ Use misting system to increase humidity")
                if 'high' in issue_lower:
                    recommendations.append("🍄 Monitor for fungal diseases")
    
    # Add general recommendations
    if stress_level in ["Critical", "High"]:
        recommendations.append("📱 Monitor every 6 hours")
        recommendations.append("📸 Document changes with photos")
        recommendations.append("👨‍🌾 Consider expert consultation")
    else:
        recommendations.append("📅 Continue regular monitoring")
        recommendations.append("📊 Maintain data records")
        recommendations.append("🔍 Watch for early signs of stress")
    
    # Remove duplicates while preserving order
    unique_recommendations = []
    for rec in recommendations:
        if rec not in unique_recommendations:
            unique_recommendations.append(rec)
    
    print(f"💡 Generated {len(unique_recommendations)} recommendations")
    return stress_level, analysis, issues, unique_recommendations

@app.route('/api/sensor-data', methods=['POST'])
def receive_sensor_data():
    """Receive data from ESP32"""
    try:
        print("\n" + "="*50)
        print("📨 RECEIVING SENSOR DATA FROM ESP32")
        print("="*50)
        
        data = request.json
        print(f"📊 Raw data: {data}")
        
        # Validate data
        required_fields = ['soil_moisture', 'soil_moisture_percent', 'temperature', 'humidity']
        for field in required_fields:
            if field not in data:
                error_msg = f"Missing field: {field}"
                print(f"❌ Validation error: {error_msg}")
                return jsonify({"status": "error", "message": error_msg}), 400
        
        print("✅ Data validation passed")
        
        # Analyze crop data
        stress_level, analysis, issues, recommendations = analyze_crop_data(
            data['soil_moisture'],
            data['soil_moisture_percent'],
            data['temperature'],
            data['humidity']
        )
        
        print(f"📈 Analysis result - Stress Level: {stress_level}")
        
        # Store in database
        print("💾 Saving to database...")
        conn = sqlite3.connect('crop_data.db')
        c = conn.cursor()
        c.execute('''INSERT INTO sensor_data 
                    (soil_moisture, soil_moisture_percent, temperature, humidity, 
                     stress_level, analysis, recommendations)
                    VALUES (?, ?, ?, ?, ?, ?, ?)''',
                 (data['soil_moisture'], data['soil_moisture_percent'],
                  data['temperature'], data['humidity'],
                  stress_level, analysis, json.dumps(recommendations)))
        conn.commit()
        
        # Get the inserted ID
        c.execute('SELECT last_insert_rowid()')
        row_id = c.fetchone()[0]
        conn.close()
        
        print(f"✅ Data saved to database (ID: {row_id})")
        
        # Prepare response
        response = {
            "status": "success",
            "message": "Data received and analyzed",
            "stress_level": stress_level,
            "issues": issues,
            "ai_analysis": analysis,
            "recommendations": recommendations,
            "timestamp": datetime.now().isoformat()
        }
        
        print(f"📤 Sending response: {response['status']}")
        print("="*50)
        
        return jsonify(response)
        
    except Exception as e:
        print(f"\n❌ CRITICAL ERROR: {str(e)}")
        print("Stack trace:")
        import traceback
        traceback.print_exc()
        print("="*50)
        return jsonify({"status": "error", "message": str(e)}), 500

@app.route('/api/current', methods=['GET'])
def get_current():
    """Get latest data"""
    try:
        print("\n📡 GET /api/current request")
        
        conn = sqlite3.connect('crop_data.db')
        c = conn.cursor()
        c.execute('''SELECT * FROM sensor_data 
                     ORDER BY timestamp DESC LIMIT 1''')
        row = c.fetchone()
        conn.close()
        
        if row:
            print(f"✅ Found data in database (ID: {row[0]})")
            
            # Handle old rows that might not have all columns
            try:
                recommendations = json.loads(row[8]) if len(row) > 8 and row[8] else []
            except:
                recommendations = []
                
            try:
                analysis = row[7] if len(row) > 7 else "Analysis not available"
            except:
                analysis = "Analysis not available"
            
            response = {
                "timestamp": row[1],
                "soil_moisture": row[2],
                "soil_moisture_percent": row[3],
                "temperature": row[4],
                "humidity": row[5],
                "stress_level": row[6],
                "ai_analysis": analysis,
                "recommendations": recommendations
            }
            
            print(f"📊 Sending data: Stress={response['stress_level']}, Soil={response['soil_moisture_percent']}%")
            return jsonify(response)
        
        print("ℹ️ No data in database yet")
        # Return default data if no data in database
        return jsonify({
            "status": "waiting",
            "message": "Waiting for sensor data",
            "soil_moisture": 0,
            "soil_moisture_percent": 50,
            "temperature": 25.0,
            "humidity": 60.0,
            "stress_level": "Low",
            "ai_analysis": "System is ready. Waiting for sensor data from ESP32...",
            "recommendations": ["Connect ESP32 to start monitoring", "Check sensor connections"]
        })
        
    except Exception as e:
        print(f"❌ Error in /api/current: {str(e)}")
        return jsonify({"error": str(e)}), 500

@app.route('/api/history', methods=['GET'])
def get_history():
    """Get historical data"""
    try:
        print("\n📜 GET /api/history request")
        
        conn = sqlite3.connect('crop_data.db')
        c = conn.cursor()
        c.execute('''SELECT * FROM sensor_data 
                     ORDER BY timestamp DESC LIMIT 100''')
        rows = c.fetchall()
        conn.close()
        
        print(f"📊 Found {len(rows)} historical records")
        
        history = []
        for row in rows:
            try:
                recommendations = json.loads(row[8]) if len(row) > 8 and row[8] else []
            except:
                recommendations = []
                
            try:
                analysis = row[7] if len(row) > 7 else "Analysis not available"
            except:
                analysis = "Analysis not available"
            
            history.append({
                "id": row[0],
                "timestamp": row[1],
                "soil_moisture": row[2],
                "soil_moisture_percent": row[3],
                "temperature": row[4],
                "humidity": row[5],
                "stress_level": row[6],
                "ai_analysis": analysis,
                "recommendations": recommendations
            })
        
        return jsonify(history)
    except Exception as e:
        print(f"❌ Error in /api/history: {str(e)}")
        return jsonify({"error": str(e)}), 500

@app.route('/')
def index():
    print("\n🌐 Web UI accessed")
    return render_template('index.html')

@app.route('/api/debug', methods=['GET'])
def debug():
    """Debug endpoint to check database status"""
    try:
        print("\n🔧 DEBUG ENDPOINT")
        
        conn = sqlite3.connect('crop_data.db')
        c = conn.cursor()
        
        # Check table info
        c.execute("PRAGMA table_info(sensor_data)")
        columns = c.fetchall()
        
        # Count records
        c.execute("SELECT COUNT(*) FROM sensor_data")
        count = c.fetchone()[0]
        
        # Get latest record
        c.execute("SELECT * FROM sensor_data ORDER BY timestamp DESC LIMIT 1")
        latest = c.fetchone()
        
        conn.close()
        
        return jsonify({
            "database": "crop_data.db",
            "table_columns": [{"name": col[1], "type": col[2]} for col in columns],
            "total_records": count,
            "latest_record": latest if latest else "No records"
        })
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    print("\n" + "="*60)
    print("🌱 AGROSENSE AI CROP MONITORING SYSTEM")
    print("="*60)
    print("📡 Starting backend server...")
    print(f"🌐 Local URL: http://localhost:5000")
    print(f"🌐 Network URL: http://192.168.254.110:5000")
    print(f"📱 ESP32 POST endpoint: http://192.168.254.110:5000/api/sensor-data")
    print("="*60 + "\n")
    
    app.run(host='0.0.0.0', port=5000, debug=True)