// Get AI agent output
const aiHTML = $json.data || $json.htmlContent || $json;

// Get ESP32 sensor data from the webhook's "body"
const sensorData = $node["Esp32 Webhook"].json.body || {};

// Add timestamp
const now = new Date().toLocaleString();

// Return structured data for HTML + email
return {
  htmlContent: aiHTML,
  temperature: sensorData.temperature,
  humidity: sensorData.humidity,
  timestamp: now
};
