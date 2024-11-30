#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

// Estrutura para passar parâmetros para as threads
typedef struct {
    uint8_t *data;
    size_t start;
    size_t end;
    size_t record_size;
} SortArgs;

int compare_records(const void *a, const void *b) {
    // Comparador para os registros baseado na chave de 32 bits
    uint32_t key_a = *(uint32_t *)a;
    uint32_t key_b = *(uint32_t *)b;
    return (key_a > key_b) - (key_a < key_b);
}

void *sort_thread(void *arg) {
    // Função de ordenação que será usada pelas threads
    SortArgs *args = (SortArgs *)arg;
    qsort(args->data + args->start * args->record_size,
          args->end - args->start,
          args->record_size,
          compare_records);
    pthread_exit(NULL);
}

void merge(uint8_t *data, size_t start1, size_t end1, size_t start2, size_t end2, size_t record_size) {
    // Mescla duas partes ordenadas
    size_t len1 = end1 - start1;
    size_t len2 = end2 - start2;
    uint8_t *temp = malloc((len1 + len2) * record_size);

    size_t i = start1, j = start2, k = 0;
    while (i < end1 && j < end2) {
        if (compare_records(data + i * record_size, data + j * record_size) <= 0) {
            memcpy(temp + k * record_size, data + i * record_size, record_size);
            i++;
        } else {
            memcpy(temp + k * record_size, data + j * record_size, record_size);
            j++;
        }
        k++;
    }

    while (i < end1) {
        memcpy(temp + k * record_size, data + i * record_size, record_size);
        i++;
        k++;
    }
    while (j < end2) {
        memcpy(temp + k * record_size, data + j * record_size, record_size);
        j++;
        k++;
    }

    memcpy(data + start1 * record_size, temp, (len1 + len2) * record_size);
    free(temp);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <entrada> <saída> <threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];
    int num_threads = atoi(argv[3]);
    if (num_threads < 0) {
        fprintf(stderr, "Número de threads inválido.\n");
        return EXIT_FAILURE;
    }

    const size_t record_size = 100;
    int input_fd = open(input_file, O_RDONLY);
    if (input_fd < 0) {
        perror("Erro ao abrir arquivo de entrada");
        return EXIT_FAILURE;
    }

    struct stat st;
    if (fstat(input_fd, &st) != 0) {
        perror("Erro ao obter informações do arquivo de entrada");
        close(input_fd);
        return EXIT_FAILURE;
    }

    size_t file_size = st.st_size;
    if (file_size % record_size != 0) {
        fprintf(stderr, "Arquivo de entrada possui tamanho inválido.\n");
        close(input_fd);
        return EXIT_FAILURE;
    }

    size_t num_records = file_size / record_size;
    uint8_t *data = malloc(file_size);
    if (!data) {
        perror("Erro ao alocar memória");
        close(input_fd);
        return EXIT_FAILURE;
    }

    if (read(input_fd, data, file_size) != file_size) {
        perror("Erro ao ler arquivo de entrada");
        free(data);
        close(input_fd);
        return EXIT_FAILURE;
    }
    close(input_fd);

    if (num_threads == 0) {
        num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    }
    if (num_threads > num_records) {
        num_threads = num_records;
    }

    pthread_t threads[num_threads];
    SortArgs args[num_threads];
    size_t records_per_thread = num_records / num_threads;

    for (int i = 0; i < num_threads; i++) {
        args[i].data = data;
        args[i].start = i * records_per_thread;
        args[i].end = (i == num_threads - 1) ? num_records : (i + 1) * records_per_thread;
        args[i].record_size = record_size;
        pthread_create(&threads[i], NULL, sort_thread, &args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 1; i < num_threads; i++) {
        merge(data, 0, args[i - 1].end, args[i].start, args[i].end, record_size);
    }

    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd < 0) {
        perror("Erro ao abrir arquivo de saída");
        free(data);
        return EXIT_FAILURE;
    }

    if (write(output_fd, data, file_size) != file_size) {
        perror("Erro ao escrever no arquivo de saída");
        free(data);
        close(output_fd);
        return EXIT_FAILURE;
    }

    if (fsync(output_fd) != 0) {
        perror("Erro ao sincronizar arquivo de saída");
        free(data);
        close(output_fd);
        return EXIT_FAILURE;
    }

    free(data);
    close(output_fd);
    return EXIT_SUCCESS;
}
