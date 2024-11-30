import subprocess
import time
import numpy as np
from scipy.stats import t

# Configurações
program = "./parallel_sort"                 # Caminho para o programa
input_file = "aleatorio.dat"                # Arquivo de entrada
output_file = "saidaTeste1.out"             # Arquivo de saída
threads = 8                                 # Número de threads a serem usadas
num_executions = 100                        # Número de execuções

# Lista para armazenar os tempos
execution_times = []

print("Iniciando análise de desempenho...")

# Executando o programa múltiplas vezes
for i in range(num_executions):
    start_time = time.time()
    try:
        subprocess.run([program, input_file, output_file, str(threads)], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except subprocess.CalledProcessError as e:
        print(f"Erro na execução {i + 1}: {e}")
        continue
    end_time = time.time()
    elapsed_time = end_time - start_time
    execution_times.append(elapsed_time)
    print(f"Execução {i + 1}: {elapsed_time:.4f} segundos")

# Cálculos estatísticos
mean_time = np.mean(execution_times)
std_dev = np.std(execution_times, ddof=1)
confidence_level = 0.95
n = len(execution_times)

# Intervalo de confiança
t_critical = t.ppf((1 + confidence_level) / 2, df=n - 1)
margin_of_error = t_critical * (std_dev / np.sqrt(n))
confidence_interval = (mean_time - margin_of_error, mean_time + margin_of_error)

# Exibindo resultados
print("\nResultados:")
print(f"Número de execuções: {n}")
print(f"Tempo médio: {mean_time:.4f} segundos")
print(f"Desvio padrão: {std_dev:.4f} segundos")
print(f"Intervalo de confiança (95%): ({confidence_interval[0]:.4f}, {confidence_interval[1]:.4f}) segundos")
