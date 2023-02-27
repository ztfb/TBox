#include <windows.h>
#include "hook.h"

static HHOOK ghook = nullptr;// 全局钩子
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    QPoint point = QCursor::pos();  // 获取鼠标当前位置
    switch (wParam){
    case WM_LBUTTONDOWN:// 鼠标左键按下
        HOOK::instance()->mouseEvent(new QMouseEvent(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));break;
    case WM_LBUTTONUP:     // 鼠标左键抬起
        HOOK::instance()->mouseEvent(new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));break;
    case WM_RBUTTONDOWN:// 鼠标右键按下
        HOOK::instance()->mouseEvent(new QMouseEvent(QEvent::MouseButtonPress, point, Qt::RightButton, Qt::RightButton, Qt::NoModifier));break;
    case WM_RBUTTONUP:// 鼠标右键抬起
        HOOK::instance()->mouseEvent(new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::RightButton, Qt::RightButton, Qt::NoModifier));break;
    case WM_MOUSEMOVE:// 鼠标移动
        HOOK::instance()->mouseEvent(new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier));break;
    case WM_MOUSEWHEEL:{// 鼠标滚轮
        MSLLHOOKSTRUCT * msll = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        int delta = GET_WHEEL_DELTA_WPARAM(msll->mouseData);// 获取滚轮状态
        HOOK::instance()->wheelEvent(new QWheelEvent(point, delta, Qt::MiddleButton, Qt::NoModifier));
        break;
    }
    default:break;
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
bool HOOK::install()
{
    if(ghook)return true;// 避免重复安装
    ghook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandleW(nullptr), 0);
    return ghook;
}
bool HOOK::uninstall()
{
    if(!ghook)return true;   // 避免重复卸载
    bool ret = UnhookWindowsHookEx(ghook);
    if(ret){
        ghook = nullptr;
        return true;
    }
    return false;
}
HOOK::HOOK(QObject *parent) : QObject(parent)
{
}
