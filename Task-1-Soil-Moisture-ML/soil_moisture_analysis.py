"""
Basic machine-learning demonstration for the soil-moisture task.
Uses sample_data.csv, which contains demo/simulated temperature and humidity.
Soil moisture in the sample file is also demo data; replace it with real exported
measurements before drawing conclusions about a real plant or sensor.
"""
from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score
import numpy as np

data_path = Path(__file__).with_name("sample_data.csv")
df = pd.read_csv(data_path)

print("First five rows:")
print(df.head())
print("\nSummary:")
print(df.describe())

plt.figure()
plt.plot(df["reading"], df["soil_moisture_percent"], marker="o")
plt.xlabel("Reading number")
plt.ylabel("Soil moisture (%)")
plt.title("Demo soil-moisture readings")
plt.grid(True)
plt.tight_layout()
plt.show()

plt.figure()
plt.scatter(df["humidity_percent"], df["soil_moisture_percent"])
plt.xlabel("Simulated humidity (%)")
plt.ylabel("Demo soil moisture (%)")
plt.title("Humidity vs. soil moisture (demo data)")
plt.grid(True)
plt.tight_layout()
plt.show()

# Simple one-feature linear regression: humidity -> soil moisture.
X = df[["humidity_percent"]]
y = df["soil_moisture_percent"]
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.25, random_state=42
)

model = LinearRegression()
model.fit(X_train, y_train)
predictions = model.predict(X_test)

mse = mean_squared_error(y_test, predictions)
print("\nModel evaluation (demo data only):")
print(f"MAE:  {mean_absolute_error(y_test, predictions):.3f}")
print(f"MSE:  {mse:.3f}")
print(f"RMSE: {np.sqrt(mse):.3f}")
print(f"R-squared: {r2_score(y_test, predictions):.3f}")

plt.figure()
plt.scatter(X_test["humidity_percent"], y_test, label="Actual demo values")
order = np.argsort(X_test["humidity_percent"].to_numpy())
plt.plot(
    X_test["humidity_percent"].to_numpy()[order],
    predictions[order],
    label="Linear regression",
)
plt.xlabel("Simulated humidity (%)")
plt.ylabel("Demo soil moisture (%)")
plt.title("Linear regression (demo data)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
