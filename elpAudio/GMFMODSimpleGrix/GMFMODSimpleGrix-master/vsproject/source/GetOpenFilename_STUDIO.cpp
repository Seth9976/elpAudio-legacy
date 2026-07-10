#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <string.h>

// 指示链接器连接原生系统弹窗需要用的 comdlg32.lib
#pragma comment(lib, "comdlg32.lib")

// 定义供GM接收的安全静态字符串内存，最大4个路径深度保证不越界
static char gof_result[MAX_PATH * 4];

// cdecl 和导出声明
extern "C" __declspec(dllexport) const char* GetOpenFilename(
    const char* title,
    const char* dir,
    const char* fname,
    const char* mask)
{
    gof_result[0] = '\0'; // 初始清空

    wchar_t wtitle[512] = { 0 };
    wchar_t wdir[MAX_PATH] = { 0 };
    wchar_t wfile[MAX_PATH] = { 0 };

    // 将 GM8 的 ANSI 输入强转为 Windows 宽字符
    MultiByteToWideChar(CP_ACP, 0, title, -1, wtitle, 512);
    MultiByteToWideChar(CP_ACP, 0, dir, -1, wdir, MAX_PATH);
    MultiByteToWideChar(CP_ACP, 0, fname, -1, wfile, MAX_PATH);

    // GML 已提前用 chr(0) 替换掉了 "|"
    // 在这里找出结尾的那两个空字符 \0\0 定位其实际真实长度
    const char* p = mask;
    while (*p || *(p + 1)) p++;
    int maskBytes = (int)(p - mask) + 2;

    // 因为 Windows 支持超长复合 Filter，直接栈分配不一定安全，继续沿用宽栈/堆内存方案，此处用宽静态防止泄露
    static wchar_t wmask[2048];
    memset(wmask, 0, sizeof(wmask));
    if (maskBytes < 2047) {
        MultiByteToWideChar(CP_ACP, 0, mask, maskBytes, wmask, maskBytes);
    }

    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = GetActiveWindow(); // 获取 GM 主程序的焦点句柄
    ofn.lpstrFilter = wmask;
    ofn.lpstrInitialDir = wdir;
    ofn.lpstrTitle = wtitle;
    ofn.lpstrFile = wfile;
    ofn.nMaxFile = MAX_PATH;
    
    // OFN_NOCHANGEDIR 这是核心，严禁原生窗体跳转造成 Fmod 底层相对寻址迷失当前目录
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR; 

    // 执行对话框唤起！如果玩家不点[取消]，则走这里的回调写入结果静态区
    if (GetOpenFileNameW(&ofn)) {
        WideCharToMultiByte(CP_ACP, 0, wfile, -1, gof_result, sizeof(gof_result), NULL, NULL);
    }

    // 弹射返回值给 GML 环境解析
    return gof_result;
}
