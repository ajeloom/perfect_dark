#include <windows.h>

#ifdef __cplusplus

VOID InputCommand();
bool Initialize();
void Message(std::string message);
int IsConnected();
void QueueItem(APClient::NetworkItem item);

extern "C" {
#endif

void AP_Init();
void AP_Close();
void PollServer();
void ReachedGoal();
void InternalCollectAPItem(uint64_t location);
char SendDeathLink();
const char *GetServerAddress();
const char *GetSlotName();
const char *GetPassword();
const char *GetStatus();

#ifdef __cplusplus
}
#endif
