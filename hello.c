#include <efi.h>
#include <efilib.h>

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    Print(L"Hello World from a Secure Boot signed EFI binary!\n");
    Print(L"Press any key (auto-continues in 10 seconds)...\n");

    EFI_BOOT_SERVICES *BS = SystemTable->BootServices;
    EFI_EVENT timer;
    EFI_EVENT events[2];
    UINTN index;

    /* Create a 10-second one-shot timer */
    BS->CreateEvent(EVT_TIMER, 0, NULL, NULL, &timer);
    BS->SetTimer(timer, TimerRelative, 100000000ULL);   /* units of 100 ns => 10 s */

    /* Wait on EITHER a keypress OR the timer, whichever comes first */
    events[0] = SystemTable->ConIn->WaitForKey;
    events[1] = timer;
    BS->WaitForEvent(2, events, &index);

    if (index == 0) {
        EFI_INPUT_KEY Key;
        SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
        Print(L"Key received. Continuing.\n");
    } else {
        Print(L"Timed out. Continuing.\n");
    }

    BS->CloseEvent(timer);
    return EFI_SUCCESS;
}
