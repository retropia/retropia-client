#include "config.h"
#include "core/common/logger.h"
#include "core/common/file_processor.h"
#include "qt/client.h"
#include "qt/ui/client_window.h"

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#include <breakpad/client/windows/handler/exception_handler.h>
#include <windows.h>
#endif

r_logger_t r_logger = NULL;
google_breakpad::ExceptionHandler *exception_handler = NULL;

void on_application_exit(void)
{
    r_logger_print(r_logger, R_LOG_INFO, "Application terminating");

    if (r_logger)
    {
        r_logger_destroy(r_logger);
    }
}

bool on_exception(const wchar_t *dump_path, const wchar_t *dump_id, void *ctx, EXCEPTION_POINTERS *exinfo, MDRawAssertionInfo *assertion, bool succeeded)
{
    LPWSTR errMsg = NULL;
    HMODULE hNtDll;

    hNtDll = GetModuleHandle(L"NTDLL.DLL");
    if (hNtDll)
    {
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS, hNtDll, exinfo->ExceptionRecord->ExceptionCode, 0, (LPWSTR)&errMsg, 0, NULL);
    }

    if (errMsg)
    {
        r_logger_wprint(r_logger, R_LOG_FATAL, L"A fatal exception occurred (%s)!", errMsg);
        LocalFree(errMsg);
    }
    else
    {
        r_logger_print(r_logger, R_LOG_FATAL, "A fatal exception occurred!");
    }

    if (succeeded)
    {
        r_logger_wprint(r_logger, R_LOG_INFO, L"A crash dump was saved to %s\\%s.dmp", dump_path, dump_id);
    }
    else
    {
        r_logger_print(r_logger, R_LOG_ERROR, "Unable to save crash dump!");
    }

    return succeeded;
}

void on_application_start(void)
{
    r_logger = r_logger_create();
    r_logger_add_stream(r_logger, stdout, R_LOG_DEBUG);
    r_logger_add_file(r_logger, ".\\retropia.log", R_LOG_DEBUG);
    r_logger_print(r_logger, R_LOG_DEBUG, "Application starting");
}

int main(int argc, char **argv)
{
    atexit(on_application_exit);
    on_application_start();

    google_breakpad::ExceptionHandler eh(L".", NULL, &on_exception, NULL, google_breakpad::ExceptionHandler::HANDLER_ALL);

    retropia::Client client(argc, argv);
    retropia::ui::ClientWindow window;
    window.show();

    int status = client.exec();

    return status;
}
