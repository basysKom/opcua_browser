/**
 * SPDX-FileCopyrightText: 2024 basysKom GmbH
 * SPDX-FileContributor: Karsten Herrler <karsten.herrler@basyskom.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QGuiApplication>
#include <QScreen>
#include <QWindow>
#ifdef Q_OS_ANDROID
#  include <QtCore/private/qandroidextras_p.h>
#endif

#include "uisettings.h"

/* ************************************************************************** */

// WindowManager.LayoutParams
#define FLAG_TRANSLUCENT_STATUS 0x04000000
#define FLAG_TRANSLUCENT_NAVIGATION 0x08000000
#define FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS 0x80000000

// View
#define SYSTEM_UI_FLAG_LIGHT_STATUS_BAR 0x00002000
#define SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR 0x00000010

// WindowInsetsController
#define APPEARANCE_LIGHT_STATUS_BARS 0x00000008
#define APPEARANCE_LIGHT_NAVIGATION_BARS 0x00000010

/* ************************************************************************** */

#ifdef Q_OS_ANDROID
static bool isLightTheme = false;
static bool areRefreshSlotsConnected = false;

static bool isQColorLight(const QColor &color)
{
    double darkness =
            1.0 - (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue()) / 255.0;
    return (darkness < 0.2);
}
#endif

#ifdef Q_OS_ANDROID
static QJniObject getAndroidWindow()
{
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

    return window;
}

static QJniObject getAndroidDecorView()
{
    QJniObject window = getAndroidWindow();
    QJniObject view = window.callObjectMethod("getDecorView", "()Landroid/view/View;");

    return view;
}
#endif

UiSettings::UiSettings(QObject *parent) : QObject{ parent } { }

void UiSettings::setStatusAndNavigationBarColor(const QColor &color)
{
#ifdef Q_OS_ANDROID
    isLightTheme = isQColorLight(color);
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([=]() {
        QJniObject window = getAndroidWindow();
        window.callMethod<void>("addFlags", "(I)V", FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        window.callMethod<void>("clearFlags", "(I)V",
                                FLAG_TRANSLUCENT_STATUS | FLAG_TRANSLUCENT_NAVIGATION);

        // Desired status bar color
        window.callMethod<void>("setStatusBarColor", "(I)V", color.rgba());
        // Desired navigation bar color
        window.callMethod<void>("setNavigationBarColor", "(I)V", color.rgba());

        setStatusAndNavigationBarTheme(isLightTheme);
    });
#else
    Q_UNUSED(color)
#endif
}

void UiSettings::setStatusAndNavigationBarTheme(bool isLight)
{
#ifdef Q_OS_ANDROID
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([=]() {
        if (QNativeInterface::QAndroidApplication::sdkVersion() < 30) {
            // Added in API level 23 // Deprecated in API level 30

            QJniObject view = getAndroidDecorView();

            const int barsUiFlags =
                    SYSTEM_UI_FLAG_LIGHT_STATUS_BAR | SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR;
            int visibility = view.callMethod<int>("getSystemUiVisibility", "()I");
            if (isLight)
                visibility |= barsUiFlags;
            else
                visibility &= ~barsUiFlags;

            view.callMethod<void>("setSystemUiVisibility", "(I)V", visibility);
        } else if (QNativeInterface::QAndroidApplication::sdkVersion() >= 30) {
            // Added in API level 30

            QJniObject window = getAndroidWindow();
            QJniObject inset = window.callObjectMethod("getInsetsController",
                                                       "()Landroid/view/WindowInsetsController;");

            const int barsAppearance =
                    APPEARANCE_LIGHT_STATUS_BARS | APPEARANCE_LIGHT_NAVIGATION_BARS;
            int visibility = inset.callMethod<int>("getSystemBarsAppearance", "()I");
            if (isLight)
                visibility |= barsAppearance;
            else
                visibility &= ~barsAppearance;

            inset.callMethod<void>("setSystemBarsAppearance", "(II)V", visibility, barsAppearance);

            if (!areRefreshSlotsConnected) {
                const QScreen *screen = qApp->primaryScreen();
                if (screen) {
                    QObject::connect(screen, &QScreen::orientationChanged, qApp,
                                     [](Qt::ScreenOrientation) {
                                         setStatusAndNavigationBarTheme(isLightTheme);
                                     });
                }

                const QWindowList windows = qApp->allWindows();
                if (windows.size() && windows.at(0)) {
                    const QWindow *window_qt = windows.at(0);
                    QObject::connect(window_qt, &QWindow::visibilityChanged, qApp,
                                     [](QWindow::Visibility) {
                                         setStatusAndNavigationBarTheme(isLightTheme);
                                     });
                }

                areRefreshSlotsConnected = true;
            }
        }
    });
#else
    Q_UNUSED(isLight)
#endif
}
