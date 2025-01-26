# 🌞 Dual Axis Solar Tracker Power Prediction

## Tech Stack
- **Python**
- **Arduino**
- **ESP32**
- **Random Forest**
- **Machine Learning**
- **IoT**

---

## 📖 Description
This project implements a dual-axis solar tracker system with integrated power prediction capabilities. It uses advanced IoT hardware, machine learning models, and data analysis tools to optimize solar energy harvesting. 

### Key Components:
1. **IoT and Sensors**:
   - DHT11: Measures temperature and humidity.
   - BH1750: Measures light intensity.
   - MPU6050: Monitors orientation and movement.
   - ACS712: Tracks current for power monitoring.
   
2. **Embedded Systems**:
   - Arduino and ESP32: Used for hardware control and data collection.

3. **Machine Learning Models**:
   - Linear Regression
   - Random Forest Regressor
   - Decision Tree Regressor  
   These models predict solar power generation based on historical and real-time data.

4. **Weather Integration**:
   - Weather data from OpenWeatherMap API enhances prediction accuracy.
   - Historical weather data from Antwerp used for power prediction experiments.

5. **Data Processing and Analysis**:
   - Files `converter.py` and `final.py`:
     - `converter.py`: Handles dataset processing, Random Forest model training, and feature extraction for IoT integration.
     - `final.py`: Merges weather and power data, builds regression models, and provides detailed data analysis and visualization.

6. **Visualization**:
   - Data preprocessing, visualization, and analysis using:
     - Pandas
     - Matplotlib
     - Seaborn

---

## 🚀 Features
- Real-time sensor data acquisition.
- Dual-axis solar tracker for optimized sunlight absorption.
- Machine learning-based solar power prediction.
- Weather data integration for enhanced prediction.
- Data visualization for daily solar power trends.

---

## 📂 File Descriptions
- **`File converter.ipynb`**:
  - Data processing and Random Forest model training.
  - Generates feature importance and exports model headers for embedded systems.

- **`File converter.ipynb`**:
  - Merges historical weather and power datasets.
  - Builds regression models and evaluates performance.
  - Visualizes weather conditions and solar power trends.

---

## 🛠️ Installation and Usage
1. Clone the repository:
   ```bash
   git clone https://github.com/kavindu26589/Dual-Axis-Solar-Tracker-Project.git
