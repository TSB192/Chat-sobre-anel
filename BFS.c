char* FindPathToNode(struct Node *startNode, struct Node *destinationNode) {
    struct Queue q;
    initQueue(&q);

    char *path = (char *)malloc(64 * sizeof(char));
    if (path == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    bool visited[MAX_NODES] = {false};  // Inicializa todos os nós como não visitados
    struct Node *prevNode[MAX_NODES];    // Array para rastrear o nó predecessor de cada nó

    // Inicialização da busca em largura
    enqueue(&q, startNode);
    visited[startNode->id] = true;
    prevNode[startNode->id] = NULL;

    while (!isEmpty(&q)) {
        struct Node *currentNode = dequeue(&q);

        // Verifica se chegamos ao nó de destino
        if (currentNode == destinationNode) {
            // Constrói o caminho reverso
            struct Node *temp = currentNode;
            int length = 0;
            while (temp != NULL) {
                length += snprintf(NULL, 0, "%d", temp->id); // Obtem o comprimento do próximo número
                temp = prevNode[temp->id];
                if (temp != NULL)
                    length += 2; // Inclui espaço e hífen
            }

            // Alocar espaço suficiente para o caminho
            path = (char*)realloc(path, (length + 1) * sizeof(char));
            if (path == NULL) {
                printf("Memory reallocation failed.\n");
                exit(EXIT_FAILURE);
            }

            // Construir o caminho reverso
            temp = currentNode;
            path[0] = '\0'; // Garante que a string seja inicializada corretamente
            while (temp != NULL) {
                char temp_str[64];
                sprintf(temp_str, "%d", temp->id);
                strcat(path, temp_str);
                temp = prevNode[temp->id];
                if (temp != NULL)
                    strcat(path, "-");
            }
            // Inverte o caminho
            int len = strlen(path);
            for (int i = 0; i < len / 2; ++i) {
                char temp = path[i];
                path[i] = path[len - i - 1];
                path[len - i - 1] = temp;
            }
            return path;
        }

        // Enfileira os vizinhos não visitados
        if (currentNode->predecessor && !visited[currentNode->predecessor->id]) {
            enqueue(&q, currentNode->predecessor);
            visited[currentNode->predecessor->id] = true;
            prevNode[currentNode->predecessor->id] = currentNode;
        }
        if (currentNode->successor && !visited[currentNode->successor->id]) {
            enqueue(&q, currentNode->successor);
            visited[currentNode->successor->id] = true;
            prevNode[currentNode->successor->id] = currentNode;
        }
        if (currentNode->chord && !visited[currentNode->chord->id]) {
            enqueue(&q, currentNode->chord);
            visited[currentNode->chord->id] = true;
            prevNode[currentNode->chord->id] = currentNode;
        }
    }

    // Se chegarmos aqui, não há caminho entre os nós
    sprintf(path, "Caminho não encontrado.\n");
    return path;
}