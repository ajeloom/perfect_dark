#include <PR/ultratypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void setAPConnectionText(char *text);
void SaveLoginInfo();
char *GetSavedAddress();
char *GetSavedSlotName();
char *GetSavedPassword();
int GetItemIndex();
void SetItemIndex(s32 index);

#ifdef __cplusplus
}
#endif