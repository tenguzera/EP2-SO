# EP2-SO
O programa desenvolvido se trata de um ordenador paralelo que utiliza os pthreads do Linux para garantir um alto desempenho.
Também está incluido um script Python para fazer a análise de desempenho do ordenador e outros 2 programas C que geram entradas e verificam se a ordenação foi feita corretamente.

Compile utilizando:
```$ gcc -Wall -Werror -pthread -std=c11 -O psort14747274 psort14747274.c```

Modo de uso:
```$ ./psort<nusp> <entrada> <saída> <threads>```

O parâmetro <entrada> é o nome de um arquivo que contém os registros a serem
ordenados. Cada registro tem o tamanho fixo de 100 bytes. Cada registro é composto por
uma chave representada por um inteiro de 32 bits (4 bytes) seguidos por 96 bytes de dados.
Os registros são ordenados de acordo com as chaves apenas.

O resultado da ordenação é escrito no arquivo indicado por <saída>.

O parâmetro <threads> é usado para determinar a quantidade máxima de threads
adicionais que o programa usará. Se o parâmetro for igual a 0, o programa deve decide
quantas threads usar automaticamente.

Para rodar o script de análise de desempenho é necessário ter o Python3 instalado e a seguinte biblioteca:

```$ pip install numpy scipy```

Os parâmetros devem ser editados individualmente dentro do programa, indicando o nome do arquivo de entrada, saída, o número de threads a serem utilizadas e quantas vezes será rodados.
As estatíscas obtidas são o tempo médio, o desvio padrão e o intervalo de confiança (95%).

```$ python3 calculaTempo.py```

Os programas C que geram entradas e verificam se a ordenação foi feita corretamente mostram como utilizá-los na execução.
