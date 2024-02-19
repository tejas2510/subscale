import matplotlib.pyplot as plt
import pandas as pd

# Read the data from the CSV file
data = pd.read_csv("your_data.csv")

# Separate timestamps and LDR values
timestamps = data["Timestamp"]
ldr_values = data[["LDR1", "LDR2", "LDR3", "LDR4", "LDR5", "LDR6"]]

# Define color palette based on LDR name (you can choose different colors)
colors = ['tab:blue', 'tab:orange', 'tab:green', 'tab:red', 'tab:purple', 'tab:brown']

# Plot time vs. analog value for each LDR (unchanged)
for i in range(len(ldr_values.columns)):
    plt.figure(i + 1)
    plt.plot(timestamps, ldr_values.iloc[:, i], label="LDR {}".format(i + 1))
    plt.xlabel("Time (ms)")
    plt.ylabel("Analog Value")
    plt.title("LDR {} Values over Time".format(i + 1))
    plt.show()

# Assuming you have g-force data in a separate file:
gforce_data = pd.read_csv("gforce_data.csv")

# Plot g-force vs. analog value for each LDR (unchanged)
for i in range(len(ldr_values.columns)):
    plt.figure(i + 7)
    plt.plot(gforce_data["gforce"], ldr_values.iloc[:, i], label="LDR {}".format(i + 1))
    plt.xlabel("G-Force")
    plt.ylabel("Analog Value")
    plt.title("LDR {} Values vs. G-Force".format(i + 1))
    plt.show()

# Create bar graph for average LDR values at every timestamp (unchanged)
average_ldr_values = ldr_values.mean(axis=1)
plt.figure(len(ldr_values.columns) + 8)
plt.bar(timestamps, average_ldr_values, label="Average LDR Value")
plt.xlabel("Time (ms)")
plt.ylabel("Average LDR Value")
plt.title("Average LDR Values over Time")
plt.legend()
plt.show()

# Choose timestamps and customize individual LDR bar graphs
specific_timestamps = [1000, 3000, 5000]  # Modify these timestamps as needed
for i in range(len(ldr_values.columns)):
    ldr_name = "LDR {}".format(i + 1)
    plt.figure(i + len(ldr_values.columns) + 9)
    for timestamp, color in zip(specific_timestamps, colors):
        value = ldr_values.loc[timestamps == timestamp, ldr_name].iloc[0]
        plt.bar(ldr_name, value, label="Time: {}ms".format(timestamp), color=color)
    plt.xlabel("LDR Name")
    plt.ylabel("Analog Value")
    plt.title("LDR {} Values at Specific Timesteps".format(ldr_name))
    plt.legend()
    plt.show()

# Add more bar graphs based on your analysis needs (suggestions below)
# Example: Bar graph of maximum LDR value at each timestamp
max_ldr_values = ldr_values.max(axis=1)
plt.figure(len(ldr_values.columns) + 14)
plt.bar(timestamps, max_ldr_values, label="Maximum LDR Value")
plt.xlabel("Time (ms)")
plt.ylabel("Maximum LDR Value")
plt.title("Maximum LDR Values over Time")
plt.legend()
plt.show()

# Example: Stacked bar graph for individual LDR values at each timestamp
def create_stacked_bar_graph(timestamps, ldr_values, specific_timestamps, colors):


    for timestamp in specific_timestamps:
        # Calculate cumulative sums for stacking:
        cumulative_sums = [0] * len(ldr_values.columns)
        for i in range(len(ldr_values.columns)):
            cumulative_sums[i] = sum(ldr_values.loc[timestamps == timestamp].iloc[0][:i])

        # Create the plot:
        plt.figure()
        for i, (ldr_name, value) in enumerate(zip(ldr_values.columns, ldr_values.loc[timestamps == timestamp].values[0])):
            plt.bar(ldr_name, value, bottom=cumulative_sums[i], label=ldr_name, color=colors[i])

        # Customize the plot:
        plt.xlabel("LDR Name")
        plt.ylabel("Analog Value")
        plt.title("LDR Values at Time: {}ms".format(timestamp))
        plt.legend()
        plt.show()

# Example usage:
create_stacked_bar_graph(timestamps, ldr_values, specific_timestamps, colors)