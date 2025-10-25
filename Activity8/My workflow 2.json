{
  "name": "My workflow 2",
  "nodes": [
    {
      "parameters": {
        "httpMethod": "POST",
        "path": "1631a44d-f708-4561-956b-d739481dc286",
        "options": {}
      },
      "type": "n8n-nodes-base.webhook",
      "typeVersion": 2.1,
      "position": [
        624,
        0
      ],
      "id": "7d799598-7157-46a3-815c-ddba55966b28",
      "name": "Webhook",
      "webhookId": "1631a44d-f708-4561-956b-d739481dc286"
    },
    {
      "parameters": {
        "promptType": "define",
        "text": "=You are an ESP32 sensor data interpreter. Analyze the sensor readings and provide a conversational interpretation of the data.\n\nCurrent sensor readings:\n- Temperature: {{ $json.body.temperature }}°C\n- Humidity: {{ $json.body.humidity }}%\n\nBased on these readings, provide:\n\n1. **Status Assessment**: Is the environment normal, too hot/cold, too humid/dry?\n2. **Conversational Summary**: A friendly, natural-sounding description of the current conditions\n3. **Recommendations**: Simple suggestions if needed (like \"might want to open a window\" etc.)\n4. **Mood/Feeling**: How the environment \"feels\" (cozy, comfortable, stuffy, etc.)\n\nKeep it brief, conversational, and human-like. Return as JSON format.",
        "options": {}
      },
      "type": "@n8n/n8n-nodes-langchain.agent",
      "typeVersion": 2.2,
      "position": [
        832,
        0
      ],
      "id": "2b863bf0-4c84-4b81-abed-f3613561a475",
      "name": "AI Agent",
      "notesInFlow": false
    },
    {
      "parameters": {
        "modelName": "=models/gemini-2.5-flash",
        "options": {}
      },
      "type": "@n8n/n8n-nodes-langchain.lmChatGoogleGemini",
      "typeVersion": 1,
      "position": [
        704,
        208
      ],
      "id": "2c637960-f6c8-4d7b-9518-4c25e9c090a0",
      "name": "Google Gemini Chat Model",
      "credentials": {
        "googlePalmApi": {
          "id": "oMMzJPkVokIN2NX9",
          "name": "Google Gemini(PaLM) Api account"
        }
      },
      "notes": "Create an email alert for ESP32 sensor data:\n\nTemperature: {{ $json.body.temperature }}°C\nHumidity: {{ $json.body.humidity }}%\n\nFormat this as a professional email notification using the actual values provided."
    },
    {
      "parameters": {
        "jsCode": "const aiOutput = $input.all()[0].json.output;\n\n// Extract JSON from the output\nconst jsonString = aiOutput.replace(/```json\\n|\\n```/g, '');\nconst interpretation = JSON.parse(jsonString);\n\n// Return in the correct format for HTML node\nreturn {\n  status: interpretation.status_assessment,\n  summary: interpretation.conversational_summary,\n  recommendation: interpretation.recommendations,\n  mood: interpretation.mood_feeling\n};"
      },
      "type": "n8n-nodes-base.code",
      "typeVersion": 2,
      "position": [
        1136,
        0
      ],
      "id": "36bb98a4-591f-447c-a767-cac93e8ad1d3",
      "name": "Code in JavaScript"
    },
    {
      "parameters": {
        "html": "<style>\nbody {\n    font-family: 'Arial', sans-serif;\n    max-width: 600px;\n    margin: 0 auto;\n    padding: 20px;\n    background-color: #f5f5f5;\n}\n.dashboard {\n    background: white;\n    border-radius: 10px;\n    padding: 25px;\n    box-shadow: 0 2px 10px rgba(0,0,0,0.1);\n}\n.status-header {\n    color: #e67e22;\n    border-left: 4px solid #e67e22;\n    padding-left: 15px;\n    margin-bottom: 20px;\n}\n.summary {\n    background: #fff9e6;\n    padding: 15px;\n    border-radius: 8px;\n    margin: 15px 0;\n}\n.recommendation {\n    background: #e8f4fd;\n    padding: 15px;\n    border-radius: 8px;\n    margin: 15px 0;\n}\n.mood {\n    font-style: italic;\n    color: #666;\n}\n</style>\n\n<div class=\"dashboard\">\n    <h1>🌡️ ESP32 Environment Report</h1>\n    \n    <div class=\"status-header\">\n        <h2>Current Status</h2>\n        <p><strong>{{ $json.status }}</strong></p>\n    </div>\n    \n    <div class=\"summary\">\n        <h3>💬 Quick Update</h3>\n        <p>{{ $json.summary }}</p>\n    </div>\n    \n    <div class=\"recommendation\">\n        <h3>💡 Suggestions</h3>\n        <p>{{ $json.recommendation }}</p>\n    </div>\n    \n    <div class=\"mood\">\n        <p><strong>How it feels:</strong> {{ $json.mood }}</p>\n    </div>\n</div>"
      },
      "type": "n8n-nodes-base.html",
      "typeVersion": 1.2,
      "position": [
        1328,
        0
      ],
      "id": "f88047b2-0fee-4e41-8bd7-0bf174679398",
      "name": "HTML"
    }
  ],
  "pinData": {},
  "connections": {
    "Webhook": {
      "main": [
        [
          {
            "node": "AI Agent",
            "type": "main",
            "index": 0
          }
        ]
      ]
    },
    "AI Agent": {
      "main": [
        [
          {
            "node": "Code in JavaScript",
            "type": "main",
            "index": 0
          }
        ]
      ]
    },
    "Google Gemini Chat Model": {
      "ai_languageModel": [
        [
          {
            "node": "AI Agent",
            "type": "ai_languageModel",
            "index": 0
          }
        ]
      ]
    },
    "Code in JavaScript": {
      "main": [
        [
          {
            "node": "HTML",
            "type": "main",
            "index": 0
          }
        ]
      ]
    }
  },
  "active": false,
  "settings": {
    "executionOrder": "v1"
  },
  "versionId": "a9fd5d46-60ae-4077-bcd7-d650b698496c",
  "meta": {
    "templateCredsSetupCompleted": true,
    "instanceId": "3229a1244143cbb6dddac91653ef618e6960fb5e5f0dd2545489ed67c8d0d50e"
  },
  "id": "i8UOVT6HGepiFAjt",
  "tags": []
}