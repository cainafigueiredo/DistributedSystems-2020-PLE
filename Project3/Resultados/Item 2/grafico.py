import re
import datetime as dt
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

date = dt.date(1,1,1)

# Definir os valores de n
n_values = [2,4,8,16,32,64,128]
elapsed_time = []

# Obter os tempos para gerar cada arquivo
for n in n_values:
        resultados = open("resultados_n_%d_r_100_k_1"%n).read().split("\n")

        end_hour,end_min,end_sec,end_ms = re.findall(r"Current time -> ([\:0-9]*) |",resultados[-2])[0].split(":")
        begin_hour,begin_min,begin_sec,begin_ms = re.findall(r"Current time -> ([\:0-9]*) |",resultados[0])[0].split(":")
        end_time = dt.time(int(end_hour),int(end_min),int(end_sec),int(end_ms)*1000)
        begin_time = dt.time(int(begin_hour),int(begin_min),int(begin_sec),int(begin_ms)*1000)
        delta_seconds = (dt.datetime.combine(date,end_time) - dt.datetime.combine(date,begin_time)).total_seconds()
        elapsed_time += [delta_seconds]
        
data = pd.DataFrame(data = {"Number of Processes":n_values,"Elapsed Time (s)":elapsed_time})
sns.lineplot(x="Number of Processes",y="Elapsed Time (s)",data=data)
plt.title("Tempo necessário para gerar o arquivo por completo")
plt.show()
