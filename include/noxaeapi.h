#ifndef NOXAEAPI_H
#define NOXAEAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define NOXAEAPI_VERSION "0.7.0"

typedef struct {
    char base_url[512];
    char api_key[256];
    char node_id[128];
    long timeout_seconds;
} noxaeapi_client_t;

typedef struct {
    int status_code;
    char* body;
    char error_message[256];
} noxaeapi_response_t;

// --- Client Lifecycle ---
noxaeapi_client_t* noxaeapi_client_create(const char* base_url, const char* api_key);
void noxaeapi_client_set_node(noxaeapi_client_t* client, const char* node_id);
void noxaeapi_client_free(noxaeapi_client_t* client);
void noxaeapi_response_free(noxaeapi_response_t* resp);

// --- HTTP Transport ---
noxaeapi_response_t noxaeapi_request(const noxaeapi_client_t* client, const char* method, const char* path, const char* json_body);

// --- Players Module ---
noxaeapi_response_t noxaeapi_players_get_online(const noxaeapi_client_t* client);
noxaeapi_response_t noxaeapi_players_get(const noxaeapi_client_t* client, const char* uuid);
noxaeapi_response_t noxaeapi_players_get_offline(const noxaeapi_client_t* client, const char* uuid);
noxaeapi_response_t noxaeapi_players_kick(const noxaeapi_client_t* client, const char* uuid, const char* reason);
noxaeapi_response_t noxaeapi_players_ban(const noxaeapi_client_t* client, const char* uuid, const char* reason, const char* expires);
noxaeapi_response_t noxaeapi_players_teleport(const noxaeapi_client_t* client, const char* uuid, double x, double y, double z);
noxaeapi_response_t noxaeapi_players_send_message(const noxaeapi_client_t* client, const char* uuid, const char* message);

// --- Server Module ---
noxaeapi_response_t noxaeapi_server_get_info(const noxaeapi_client_t* client);
noxaeapi_response_t noxaeapi_server_get_health(const noxaeapi_client_t* client);
noxaeapi_response_t noxaeapi_server_get_tps(const noxaeapi_client_t* client);
noxaeapi_response_t noxaeapi_server_execute_command(const noxaeapi_client_t* client, const char* command);
noxaeapi_response_t noxaeapi_server_broadcast(const noxaeapi_client_t* client, const char* message);

// --- Economy Module ---
noxaeapi_response_t noxaeapi_economy_get_info(const noxaeapi_client_t* client);
noxaeapi_response_t noxaeapi_economy_get_balance(const noxaeapi_client_t* client, const char* uuid);
noxaeapi_response_t noxaeapi_economy_set_balance(const noxaeapi_client_t* client, const char* uuid, double amount);
noxaeapi_response_t noxaeapi_economy_add_balance(const noxaeapi_client_t* client, const char* uuid, double amount);
noxaeapi_response_t noxaeapi_economy_get_top(const noxaeapi_client_t* client, int limit, int offset);

// --- Leaderboard Module ---
noxaeapi_response_t noxaeapi_leaderboard_get_sources(const noxaeapi_client_t* client);
noxaeapi_response_t noxaeapi_leaderboard_get(const noxaeapi_client_t* client, const char* source, int limit, int offset);

// --- LuckPerms Module ---
noxaeapi_response_t noxaeapi_luckperms_get_groups(const noxaeapi_client_t* client);
noxaeapi_response_t noxaeapi_luckperms_get_user_permissions(const noxaeapi_client_t* client, const char* uuid);
noxaeapi_response_t noxaeapi_luckperms_add_user_permission(const noxaeapi_client_t* client, const char* uuid, const char* permission, bool value);

// --- NoxAuth Module ---
noxaeapi_response_t noxaeapi_noxauth_get_player(const noxaeapi_client_t* client, const char* uuid);
noxaeapi_response_t noxaeapi_noxauth_check_password(const noxaeapi_client_t* client, const char* name, const char* password);

// --- Plugins & Worlds ---
noxaeapi_response_t noxaeapi_plugins_get_all(const noxaeapi_client_t* client);
noxaeapi_response_t noxaeapi_worlds_get_all(const noxaeapi_client_t* client);
noxaeapi_response_t noxaeapi_worlds_set_time(const noxaeapi_client_t* client, const char* name, int64_t time);

// --- Skills Module ---
noxaeapi_response_t noxaeapi_skills_get_player(const noxaeapi_client_t* client, const char* uuid);

// --- Identity Module ---
noxaeapi_response_t noxaeapi_identity_resolve(const noxaeapi_client_t* client, const char* name);
noxaeapi_response_t noxaeapi_identity_get_network_servers(const noxaeapi_client_t* client);

// --- RankForge Module ---
noxaeapi_response_t noxaeapi_rankforge_get_stats(const noxaeapi_client_t* client, const char* uuid);
noxaeapi_response_t noxaeapi_rankforge_get_points(const noxaeapi_client_t* client, const char* uuid);
noxaeapi_response_t noxaeapi_rankforge_add_points(const noxaeapi_client_t* client, const char* uuid, int points);
noxaeapi_response_t noxaeapi_rankforge_get_tiers(const noxaeapi_client_t* client);
noxaeapi_response_t noxaeapi_rankforge_get_top(const noxaeapi_client_t* client, int limit, int offset);

// --- Misc Module ---
noxaeapi_response_t noxaeapi_misc_ping(const noxaeapi_client_t* client);
noxaeapi_response_t noxaeapi_misc_health(const noxaeapi_client_t* client);

#ifdef __cplusplus
}
#endif

#endif // NOXAEAPI_H
