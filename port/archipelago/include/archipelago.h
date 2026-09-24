#include <windows.h>

#ifdef __cplusplus

VOID InputCommand();
bool Initialize();
void Message(std::string message);
void QueueItem(APClient::NetworkItem item);

extern "C" {
#endif

void APInitConsole();
void APCloseConsole();
int IsConnected();
int IsDisconnected();
int IsConnecting();
void PollServer();
void ReachedGoal();
void InternalCollectAPItem(uint64_t location);
char SendDeathLink();
void SetServerAddress(char *address);
void SetSlotName(char *name);
void SetPassword(char *newPassword);
char *GetServerAddress();
char *GetSlotName();
char *GetPassword();
char *GetStatus();
void PrintCommands();
void ConnectAP();
void DisconnectAP(bool joining);
int CheckIfLocationExists(int location);
int IsLocationCompleted(int location);

#ifdef __cplusplus
}
#endif
