#ESP32 code

#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 4        // GPIO4 connected to DHT11
#define DHTTYPE DHT11   // DHT11 sensor

const char* ssid = "GlobeAtHome_fbbd0_2.4";
const char* password = "D2kDeDWc";
const char* serverName = "http://192.168.254.119:5000/api/data"; // change this!

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT11!");
      delay(2000);
      return;
    }

    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"temperature\":" + String(temperature) +
                      ",\"humidity\":" + String(humidity) + "}";

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      Serial.println("✅ Data sent successfully!");
      Serial.println(http.getString());
    } else {
      Serial.print("❌ Error sending data: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi disconnected!");
  }

  delay(10000); // Send every 10 seconds
}


#Server.js Code
const express = require("express");
const mongoose = require("mongoose");
const bodyParser = require("body-parser");
const cors = require("cors");

const app = express();
app.use(cors());
app.use(bodyParser.json());

// Connect to MongoDB (make sure MongoDB is installed and running)
mongoose.connect("mongodb://localhost:27017/dht_monitor", {
  useNewUrlParser: true,
  useUnifiedTopology: true,
});
const db = mongoose.connection;
db.on("error", console.error.bind(console, "MongoDB connection error:"));

// Schema for storing temperature & humidity
const DataSchema = new mongoose.Schema({
  temperature: Number,
  humidity: Number,
  timestamp: { type: Date, default: Date.now },
});
const Data = mongoose.model("Data", DataSchema);

// POST endpoint — called by Arduino
app.post("/api/data", async (req, res) => {
  try {
    const { temperature, humidity } = req.body;
    const newData = new Data({ temperature, humidity });
    await newData.save();
    res.json({ success: true, message: "Data recorded successfully!" });
  } catch (error) {
    res.status(500).json({ success: false, message: error.message });
  }
});

// GET endpoint — view all data in browser
app.get("/api/data", async (req, res) => {
  const data = await Data.find().sort({ timestamp: -1 });
  res.json(data);
});

const PORT = 5000;
app.listen(PORT, () => console.log(`✅ Server running on port ${PORT}`));