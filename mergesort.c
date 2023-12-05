#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Estrutura para armazenar informações sobre cada sublista
typedef struct {
    int start;
    int end;
} Sublist;

// Arrays globais para armazenar a lista original e a lista mesclada
int *originalArray;
int *mergedArray;

// Função de mesclagem para o Merge Sort
void merge(int start, int mid, int end) {
    int i = start;
    int j = mid + 1;
    int k = 0;

    // Mescla as duas sub-listas ordenadas em uma lista temporária
    while (i <= mid && j <= end) {
        if (originalArray[i] < originalArray[j])
            mergedArray[k++] = originalArray[i++];
        else
            mergedArray[k++] = originalArray[j++];
    }

    // Adiciona elementos restantes da primeira sub-lista, se houver
    while (i <= mid)
        mergedArray[k++] = originalArray[i++];

    // Adiciona elementos restantes da segunda sub-lista, se houver
    while (j <= end)
        mergedArray[k++] = originalArray[j++];

    // Copia os elementos mesclados de volta para a lista original
    for (int l = 0; l < k; l++)
        originalArray[start + l] = mergedArray[l];
}

// Função principal do Merge Sort
void *mergeSort(void *arg) {
    // Converte o argumento para o tipo Sublist (estrutura que representa a sublista)
    Sublist *sublist = (Sublist *)arg;

    // Obtém os índices de início e fim da sublista
    int start = sublist->start;
    int end = sublist->end;

    // Verifica se a sublista tem mais de um elemento (caso base do Merge Sort)
    if (start < end) {
        int mid = (start + end) / 2;

        // Criar threads para ordenar cada metade
        pthread_t thread_left, thread_right;

        // Cria sublistas representando a metade esquerda e a metade direita da sublista atual
        Sublist sublist_left = {start, mid};
        Sublist sublist_right = {mid + 1, end};

        // Chama mergeSort recursivamente para cada metade
        pthread_create(&thread_left, NULL, mergeSort, &sublist_left);
        pthread_create(&thread_right, NULL, mergeSort, &sublist_right);

        // Aguarda as threads terminarem
        pthread_join(thread_left, NULL);
        pthread_join(thread_right, NULL);

        // Mescla as duas metades ordenadas
        merge(start, mid, end);
    }

    // Termina a thread
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        // Imprime uma mensagem de uso se não houver argumentos suficientes
        printf("Uso: %s <num1> <num2> <num3> ...", argv[0]);
        return 1;
    }

    int size = argc - 1;

    // Alocar memória para os arrays original e mesclado dinamicamente
    originalArray = (int *)malloc(size * sizeof(int));
    mergedArray = (int *)malloc(size * sizeof(int));

    // Verificar se a alocação foi bem-sucedida
    if (originalArray == NULL || mergedArray == NULL) {
        // Imprime uma mensagem de erro se a alocação falhar
        printf("Erro de alocação de memória.\n");
        return 1;
    }

    // Preencher o array com os números fornecidos como argumentos de linha de comando
    for (int i = 0; i < size; i++) {
        originalArray[i] = atoi(argv[i + 1]);
    }

    // Criar a estrutura para a thread principal
    Sublist mainSublist = {0, size - 1};

    // Chamar a função mergeSort para a thread principal
    pthread_t mainThread;
    pthread_create(&mainThread, NULL, mergeSort, &mainSublist);

    // Aguardar a thread principal terminar
    pthread_join(mainThread, NULL);

    // Imprimir a lista ordenada em ordem crescente
    printf("Lista ordenada em ordem crescente: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", originalArray[i]);
    }
    printf("\n");

    // Liberar a memória alocada
    free(originalArray);
    free(mergedArray);

    return 0;
}
