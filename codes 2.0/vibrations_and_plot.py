import csv
import csv
import matplotlib.pyplot as plt

def read_csv(filename):
    data = []
    with open(filename, 'r') as file:
        reader = csv.DictReader(file)
        for row in reader:
            data.append(row)
    return data

def count_transitions(data):
    transitions = {}
    time_taken = {}
    current_ldr = None
    start_time = None
    for row in data:
        for ldr, value in row.items():
            if ldr.startswith('LDR'):
                value = int(value)
                if value == 100 and ldr != current_ldr:
                    if current_ldr:
                        transitions[current_ldr + ' to ' + ldr] = transitions.get(current_ldr + ' to ' + ldr, 0) + 1
                        if start_time:
                            end_time = int(row['Timestamp'])
                            duration = end_time - start_time
                            time_taken[current_ldr + ' to ' + ldr] = time_taken.get(current_ldr + ' to ' + ldr, [])
                            time_taken[current_ldr + ' to ' + ldr].append(duration)
                            start_time = None
                    current_ldr = ldr
                    start_time = int(row['Timestamp'])
                elif value == 0 and ldr == current_ldr:
                    start_time = None
    return transitions, time_taken

data = read_csv('data.csv')
transitions, time_taken = count_transitions(data)

# Plotting time taken vs. transitions per pair
plt.figure(figsize=(10, 6))
for pair, times in time_taken.items():
    plt.scatter([pair] * len(times), times, label=pair)

plt.xlabel('Transitions per pair')
plt.ylabel('Time taken (ms)')
plt.title('Time taken vs. Transitions per pair')
plt.legend()
plt.xticks(rotation=45, ha='right')
plt.grid(True)
plt.tight_layout()
plt.show()
