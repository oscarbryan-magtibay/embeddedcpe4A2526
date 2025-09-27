async function fetchData() {
  try {
    const response = await fetch("/data?limit=1");
    const data = await response.json();

    if (data.length > 0) {
      const latest = data[0];
      document.getElementById("temp").textContent = latest.temperature.toFixed(1);
      document.getElementById("hum").textContent = latest.humidity.toFixed(1);
      document.getElementById("dev").textContent = latest.device;
      document.getElementById("time").textContent = new Date(latest.timestamp).toLocaleString();
    }
  } catch (err) {
    console.error("Error fetching data:", err);
  }
}

setInterval(fetchData, 3000);
fetchData();
