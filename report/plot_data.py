import matplotlib.pyplot as plt

times_cmb = [42.42, 32.50, 21.84, 11.31, 4.06]
EDP = [7601.49, 5143.95, 1315.92, 443.00, 73.74]
Energy = [179.20, 158.28, 60.25, 39.17, 18.16]


# Create a bar plot for EDP
plt.bar(range(len(EDP)), EDP, color="blue", alpha=0.7)

# Add value labels on top of the bars
for i, value in enumerate(EDP):
    plt.text(i, value + max(EDP) * 0.01, f"{value:.2f}", ha="center", va="bottom")

plt.ylabel("EDP")
plt.ylim(0, max(EDP) * 1.1)  # Set y-axis limit to 10% above the max EDP value
plt.title("Energy-Delay Product (EDP) for Different Optimizations")
plt.xticks(range(len(EDP)), [f"Optimization {i+1}" for i in range(len(EDP))])
plt.xticks(rotation=-45)
plt.savefig("report/EDP_plot.png", dpi=300, bbox_inches="tight")

plt.clf()  # Clear the current figure

# Create a bar plot for Time
plt.bar(range(len(times_cmb)), times_cmb, color="green", alpha=0.7)

# Add value labels on top of the bars
for i, value in enumerate(times_cmb):
    plt.text(i, value + max(times_cmb) * 0.01, f"{value:.2f}", ha="center", va="bottom")

plt.ylabel("Time (s)")
plt.ylim(0, max(times_cmb) * 1.1)  # Set y-axis limit to 10% above the max time value
plt.title("Execution Time for Different Optimizations")
plt.xticks(
    range(len(times_cmb)), [f"Optimization {i+1}" for i in range(len(times_cmb))]
)
plt.xticks(rotation=-45)
plt.savefig("report/Time_plot.png", dpi=300, bbox_inches="tight")
