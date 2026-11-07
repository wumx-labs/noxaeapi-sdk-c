#include "noxaeapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__has_include)
  #if __has_include(<curl/curl.h>)
    #define NOXAEAPI_HAS_CURL 1
    #include <curl/curl.h>
  #endif
#endif

struct curl_write_buffer {
    char* data;
    size_t size;
};

#if defined(NOXAEAPI_HAS_CURL)
static size_t curl_write_cb(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    struct curl_write_buffer* buf = (struct curl_write_buffer*)userp;
    char* ptr = (char*)realloc(buf->data, buf->size + total + 1);
    if (!ptr) return 0;
    buf->data = ptr;
    memcpy(&(buf->data[buf->size]), contents, total);
    buf->size += total;
    buf->data[buf->size] = 0;
    return total;
}
#endif

noxaeapi_client_t* noxaeapi_client_create(const char* base_url, const char* api_key) {
    noxaeapi_client_t* client = (noxaeapi_client_t*)calloc(1, sizeof(noxaeapi_client_t));
    if (!client) return NULL;

    strncpy(client->base_url, base_url ? base_url : "http://localhost:8080", sizeof(client->base_url) - 1);
    size_t len = strlen(client->base_url);
    if (len > 0 && client->base_url[len - 1] == '/') client->base_url[len - 1] = '\0';

    if (api_key) {
        strncpy(client->api_key, api_key, sizeof(client->api_key) - 1);
    }
    client->timeout_seconds = 15;
    return client;
}

void noxaeapi_client_set_node(noxaeapi_client_t* client, const char* node_id) {
    if (!client) return;
    if (node_id) {
        strncpy(client->node_id, node_id, sizeof(client->node_id) - 1);
    } else {
        client->node_id[0] = '\0';
    }
}

void noxaeapi_client_free(noxaeapi_client_t* client) {
    if (client) free(client);
}

void noxaeapi_response_free(noxaeapi_response_t* resp) {
    if (resp && resp->body) {
        free(resp->body);
        resp->body = NULL;
    }
}

noxaeapi_response_t noxaeapi_request(const noxaeapi_client_t* client, const char* method, const char* path, const char* json_body) {
    noxaeapi_response_t res;
    memset(&res, 0, sizeof(res));

    if (!client) {
        res.status_code = -1;
        snprintf(res.error_message, sizeof(res.error_message), "Null client pointer");
        return res;
    }

#if defined(NOXAEAPI_HAS_CURL)
    CURL* curl = curl_easy_init();
    if (!curl) {
        res.status_code = -1;
        snprintf(res.error_message, sizeof(res.error_message), "Failed to initialize cURL");
        return res;
    }

    char url[1024];
    snprintf(url, sizeof(url), "%s%s%s", client->base_url, (path[0] == '/' ? "" : "/"), path);

    struct curl_write_buffer buf;
    buf.data = (char*)malloc(1);
    buf.data[0] = '\0';
    buf.size = 0;

    struct curl_slist* headers = NULL;
    char auth_hdr[300];
    snprintf(auth_hdr, sizeof(auth_hdr), "X-API-Key: %s", client->api_key);
    headers = curl_slist_append(headers, auth_hdr);
    headers = curl_slist_append(headers, "Accept: application/json");

    if (client->node_id[0] != '\0') {
        char node_hdr[200];
        snprintf(node_hdr, sizeof(node_hdr), "X-Velocity-Node: %s", client->node_id);
        headers = curl_slist_append(headers, node_hdr);
    }

    if (json_body) {
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body);
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, client->timeout_seconds);

    CURLcode code = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (code != CURLE_OK) {
        res.status_code = -1;
        snprintf(res.error_message, sizeof(res.error_message), "%s", curl_easy_strerror(code));
        free(buf.data);
        return res;
    }

    res.status_code = (int)http_code;
    res.body = buf.data;
    return res;
#else
    res.status_code = 200;
    res.body = (char*)malloc(3);
    strcpy(res.body, "{}");
    return res;
#endif
}

// Players
noxaeapi_response_t noxaeapi_players_get_online(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/players", NULL);
}

noxaeapi_response_t noxaeapi_players_get(const noxaeapi_client_t* client, const char* uuid) {
    char path[128];
    snprintf(path, sizeof(path), "/players/%s", uuid);
    return noxaeapi_request(client, "GET", path, NULL);
}

noxaeapi_response_t noxaeapi_players_get_offline(const noxaeapi_client_t* client, const char* uuid) {
    char path[128];
    snprintf(path, sizeof(path), "/players/%s/offline", uuid);
    return noxaeapi_request(client, "GET", path, NULL);
}

noxaeapi_response_t noxaeapi_players_kick(const noxaeapi_client_t* client, const char* uuid, const char* reason) {
    char path[128];
    snprintf(path, sizeof(path), "/players/%s/kick", uuid);
    char body[512];
    snprintf(body, sizeof(body), "{\"reason\":\"%s\"}", reason ? reason : "Kicked");
    return noxaeapi_request(client, "POST", path, body);
}

noxaeapi_response_t noxaeapi_players_ban(const noxaeapi_client_t* client, const char* uuid, const char* reason, const char* expires) {
    char path[128];
    snprintf(path, sizeof(path), "/players/%s/ban", uuid);
    char body[512];
    if (expires) {
        snprintf(body, sizeof(body), "{\"reason\":\"%s\",\"expires\":\"%s\"}", reason ? reason : "Banned", expires);
    } else {
        snprintf(body, sizeof(body), "{\"reason\":\"%s\"}", reason ? reason : "Banned");
    }
    return noxaeapi_request(client, "POST", path, body);
}

noxaeapi_response_t noxaeapi_players_teleport(const noxaeapi_client_t* client, const char* uuid, double x, double y, double z) {
    char path[128];
    snprintf(path, sizeof(path), "/players/%s/teleport", uuid);
    char body[256];
    snprintf(body, sizeof(body), "{\"x\":%f,\"y\":%f,\"z\":%f}", x, y, z);
    return noxaeapi_request(client, "POST", path, body);
}

noxaeapi_response_t noxaeapi_players_send_message(const noxaeapi_client_t* client, const char* uuid, const char* message) {
    char path[128];
    snprintf(path, sizeof(path), "/players/%s/message", uuid);
    char body[512];
    snprintf(body, sizeof(body), "{\"message\":\"%s\"}", message ? message : "");
    return noxaeapi_request(client, "POST", path, body);
}

// Server
noxaeapi_response_t noxaeapi_server_get_info(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/server", NULL);
}

noxaeapi_response_t noxaeapi_server_get_health(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/server/health", NULL);
}

noxaeapi_response_t noxaeapi_server_get_tps(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/server/tps", NULL);
}

noxaeapi_response_t noxaeapi_server_execute_command(const noxaeapi_client_t* client, const char* command) {
    char body[512];
    snprintf(body, sizeof(body), "{\"command\":\"%s\"}", command ? command : "");
    return noxaeapi_request(client, "POST", "/server/command", body);
}

noxaeapi_response_t noxaeapi_server_broadcast(const noxaeapi_client_t* client, const char* message) {
    char body[512];
    snprintf(body, sizeof(body), "{\"message\":\"%s\"}", message ? message : "");
    return noxaeapi_request(client, "POST", "/server/broadcast", body);
}

// Economy
noxaeapi_response_t noxaeapi_economy_get_info(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/economy", NULL);
}

noxaeapi_response_t noxaeapi_economy_get_balance(const noxaeapi_client_t* client, const char* uuid) {
    char path[128];
    snprintf(path, sizeof(path), "/economy/balance/%s", uuid);
    return noxaeapi_request(client, "GET", path, NULL);
}

noxaeapi_response_t noxaeapi_economy_set_balance(const noxaeapi_client_t* client, const char* uuid, double amount) {
    char path[128];
    snprintf(path, sizeof(path), "/economy/balance/%s", uuid);
    char body[128];
    snprintf(body, sizeof(body), "{\"amount\":%f}", amount);
    return noxaeapi_request(client, "POST", path, body);
}

noxaeapi_response_t noxaeapi_economy_add_balance(const noxaeapi_client_t* client, const char* uuid, double amount) {
    char path[128];
    snprintf(path, sizeof(path), "/economy/balance/%s/add", uuid);
    char body[128];
    snprintf(body, sizeof(body), "{\"amount\":%f}", amount);
    return noxaeapi_request(client, "POST", path, body);
}

noxaeapi_response_t noxaeapi_economy_get_top(const noxaeapi_client_t* client, int limit, int offset) {
    char path[128];
    snprintf(path, sizeof(path), "/economy/top?limit=%d&offset=%d", limit, offset);
    return noxaeapi_request(client, "GET", path, NULL);
}

// Leaderboard
noxaeapi_response_t noxaeapi_leaderboard_get_sources(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/leaderboard/sources", NULL);
}

noxaeapi_response_t noxaeapi_leaderboard_get(const noxaeapi_client_t* client, const char* source, int limit, int offset) {
    char path[128];
    snprintf(path, sizeof(path), "/leaderboard/%s?limit=%d&offset=%d", source, limit, offset);
    return noxaeapi_request(client, "GET", path, NULL);
}

// LuckPerms
noxaeapi_response_t noxaeapi_luckperms_get_groups(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/luckperms/groups", NULL);
}

noxaeapi_response_t noxaeapi_luckperms_get_user_permissions(const noxaeapi_client_t* client, const char* uuid) {
    char path[128];
    snprintf(path, sizeof(path), "/luckperms/users/%s/permissions", uuid);
    return noxaeapi_request(client, "GET", path, NULL);
}

noxaeapi_response_t noxaeapi_luckperms_add_user_permission(const noxaeapi_client_t* client, const char* uuid, const char* permission, bool value) {
    char path[128];
    snprintf(path, sizeof(path), "/luckperms/users/%s/permissions", uuid);
    char body[256];
    snprintf(body, sizeof(body), "{\"permission\":\"%s\",\"value\":%s}", permission, value ? "true" : "false");
    return noxaeapi_request(client, "POST", path, body);
}

// NoxAuth
noxaeapi_response_t noxaeapi_noxauth_get_player(const noxaeapi_client_t* client, const char* uuid) {
    char path[128];
    snprintf(path, sizeof(path), "/noxauth/players/%s", uuid);
    return noxaeapi_request(client, "GET", path, NULL);
}

noxaeapi_response_t noxaeapi_noxauth_check_password(const noxaeapi_client_t* client, const char* name, const char* password) {
    char body[512];
    snprintf(body, sizeof(body), "{\"name\":\"%s\",\"password\":\"%s\"}", name, password);
    return noxaeapi_request(client, "POST", "/noxauth/check-password", body);
}

// Plugins & Worlds
noxaeapi_response_t noxaeapi_plugins_get_all(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/plugins", NULL);
}

noxaeapi_response_t noxaeapi_worlds_get_all(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/worlds", NULL);
}

noxaeapi_response_t noxaeapi_worlds_set_time(const noxaeapi_client_t* client, const char* name, int64_t time) {
    char path[128];
    snprintf(path, sizeof(path), "/worlds/%s/time", name);
    char body[128];
    snprintf(body, sizeof(body), "{\"time\":%lld}", (long long)time);
    return noxaeapi_request(client, "POST", path, body);
}

// Skills
noxaeapi_response_t noxaeapi_skills_get_player(const noxaeapi_client_t* client, const char* uuid) {
    char path[128];
    snprintf(path, sizeof(path), "/skills/players/%s", uuid);
    return noxaeapi_request(client, "GET", path, NULL);
}

// Identity
noxaeapi_response_t noxaeapi_identity_resolve(const noxaeapi_client_t* client, const char* name) {
    char path[256];
    snprintf(path, sizeof(path), "/identity/resolve?name=%s", name);
    return noxaeapi_request(client, "GET", path, NULL);
}

noxaeapi_response_t noxaeapi_identity_get_network_servers(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/identity/network/servers", NULL);
}

// RankForge
noxaeapi_response_t noxaeapi_rankforge_get_stats(const noxaeapi_client_t* client, const char* uuid) {
    char path[128];
    snprintf(path, sizeof(path), "/integrations/rankforge/players/%s", uuid);
    return noxaeapi_request(client, "GET", path, NULL);
}

noxaeapi_response_t noxaeapi_rankforge_get_points(const noxaeapi_client_t* client, const char* uuid) {
    char path[128];
    snprintf(path, sizeof(path), "/integrations/rankforge/players/%s/points", uuid);
    return noxaeapi_request(client, "GET", path, NULL);
}

noxaeapi_response_t noxaeapi_rankforge_add_points(const noxaeapi_client_t* client, const char* uuid, int points) {
    char path[128];
    snprintf(path, sizeof(path), "/integrations/rankforge/players/%s/points/add", uuid);
    char body[128];
    snprintf(body, sizeof(body), "{\"points\":%d}", points);
    return noxaeapi_request(client, "POST", path, body);
}

noxaeapi_response_t noxaeapi_rankforge_get_tiers(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/integrations/rankforge/tiers", NULL);
}

noxaeapi_response_t noxaeapi_rankforge_get_top(const noxaeapi_client_t* client, int limit, int offset) {
    char path[128];
    snprintf(path, sizeof(path), "/integrations/rankforge/top?limit=%d&offset=%d", limit, offset);
    return noxaeapi_request(client, "GET", path, NULL);
}

// Misc
noxaeapi_response_t noxaeapi_misc_ping(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/ping", NULL);
}

noxaeapi_response_t noxaeapi_misc_health(const noxaeapi_client_t* client) {
    return noxaeapi_request(client, "GET", "/health", NULL);
}
