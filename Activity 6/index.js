const express = require("express");
const { MongoClient } = require("mongodb");
const dotenv = require("dotenv");
const cors = require("cors");

dotenv.config();

const app = express();
app.use(express.json());
app.use(cors());

const client = new MongoClient(process.env.MONGODB_URI);

async function start() {
    try {
        await client.connect();
        console.log("Connected to MongoDB");

        const db = client.db("DHT_db");
        const collection = db.collection("readings");

        app.post("/api/readings", async (req, res) => {
            const data = req.body;
            data.timestamp = new Date();

            await collection.insertOne(data);
            res.json({ status: "stored", data });
        });

        app.listen(process.env.PORT || 3000, () =>
            console.log("Server running on port", process.env.PORT || 3000)
        );
    } catch (err) {
        console.error("Error:", err);
    }
}

start();
