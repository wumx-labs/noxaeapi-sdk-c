#include <stdio.h>
#include "noxaeapi.h"

int main() {
    noxaeapi_client_t* client = noxaeapi_client_create("http://localhost:8080", "test-api-key");
    if (!client) {
        fprintf(stderr, "Failed to create client\n");
        return 1;
    }

    printf("NoxAeApi C SDK v%s Initialized\n", NOXAEAPI_VERSION);

    noxaeapi_response_t resp = noxaeapi_server_get_info(client);
    printf("Server info status: %d\n", resp.status_code);
    if (resp.body) {
        printf("Body: %s\n", resp.body);
    }

    noxaeapi_response_free(&resp);
    noxaeapi_client_free(client);
    return 0;
}
