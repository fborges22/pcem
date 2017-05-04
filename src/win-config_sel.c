#define BITMAP WINDOWS_BITMAP
#include <windows.h>
#include <windowsx.h>
#undef BITMAP

#include <commctrl.h>

#include "ibm.h"
#include "config.h"
#include "resources.h"
#include "win.h"

static int has_been_inited = 0;

static void config_list_update(HWND hdlg)
{
        char s[512];
        int num, c;
        HWND h;

        strcpy(s, pcempath);
        put_backslash(s);
        strcat(s, "configs\\*.cfg");
        pclog("Dir %s\n", s);

        DlgDirList(hdlg, s, IDC_LIST, 0, 0);
        
        h = GetDlgItem(hdlg, IDC_LIST);
        num = SendMessage(h, LB_GETCOUNT, 0, 0);
        
        for (c = 0; c < num; c++)
        {
                char *ext;
                
                SendMessage(h, LB_GETTEXT, c, (LPARAM)s);
                ext = get_extension(s);
                if (ext && ext[0])
                {
                        ext--;
                        *ext = 0;
                }

                SendMessage(h, LB_DELETESTRING, c, 0);                        
                SendMessage(h, LB_INSERTSTRING, c, (LPARAM)s);
        }
}

static BOOL CALLBACK config_selection_dlgproc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
        //HWND h;

        switch (message)
        {
                case WM_INITDIALOG:
//                pause = 1;
                config_list_update(hdlg);
//                h = GetDlgItem(hdlg, IDC_LIST);
//                SendMessage(h, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)"AAA");
//                SendMessage(h, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)"BBB");
                return TRUE;
                
                case WM_COMMAND:
                switch (LOWORD(wParam))
                {
                        case IDOK:
                        {
                                char s[512];
                                
                                int ret = DlgDirSelectEx(hdlg, s, 512, IDC_LIST);
                                
                                pclog("DlgDirSelectEx returned %i %s\n", ret, s);
                                if (s[0])
                                {
                                        char cfg[512];
                                        
                                        strcpy(cfg, pcempath);
                                        put_backslash(cfg);
                                        strcat(cfg, "configs\\");
                                        strcat(cfg, s);
                                        strcat(cfg, "cfg");
//                                        sprintf(cfg, "%s\\configs\\%scfg", pcempath, s);
                                        pclog("Config name %s\n", cfg);
                                        
                                        strcpy(config_file_default, cfg);

                                        EndDialog(hdlg, 1);
//                                        pause = 0;
                                        return TRUE;
                                }
                        }
                        return FALSE;
                        
                        case IDCANCEL:
                        EndDialog(hdlg, 0);
//                        pause = 0;
                        return TRUE;
                        
                        case IDC_NEW:
                        {
                                char s[512];
                                
                                strcpy(s, pcempath);
                                put_backslash(s);
                                strcat(s, "configs\\");
                                pclog("Dir %s\n", s);

                                if (!getsfile(hdlg, "Configuration (*.CFG)\0*.CFG\0All files (*.*)\0*.*\0", "", s))
                                {
                                        config_open(hdlg, has_been_inited);
                                        
                                        saveconfig(openfilestring);
                                }

                                config_list_update(hdlg);
                                
                                return TRUE;
                        }
                }
                break;
        }
        return FALSE;
}

int config_selection_open(HWND hwnd, int inited)
{
        has_been_inited = inited;
        
        return DialogBox(hinstance, TEXT("ConfigureSelectionDlg"), hwnd, (DLGPROC)config_selection_dlgproc);
}