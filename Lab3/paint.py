import matplotlib.pyplot as plt

input_sizes = [10000, 20000, 100000, 200000, 1000000, 2000000]

cpu_times = [0.5650, 1.1260, 5.5710, 11.1200, 56.1330, 111.6490]
cuda_times = [0.0153, 0.0184, 0.0529, 0.0992, 0.4638, 0.9203]

for i in range(6):
    print(cpu_times[i]/cuda_times[i])

plt.plot(input_sizes, cpu_times, label="CPU Time (ms)", marker='o')
plt.plot(input_sizes, cuda_times, label="CUDA Time (ms)", marker='s')

plt.xlabel("Input Size")
plt.ylabel("Time (ms)")
plt.title("CPU vs CUDA Time")
plt.legend()

plt.show()
