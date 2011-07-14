// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VIEWS_DESKTOP_DESKTOP_WINDOW_H_
#define VIEWS_DESKTOP_DESKTOP_WINDOW_H_

#include "views/view.h"
#include "views/widget/widget.h"
#include "views/widget/widget_delegate.h"

namespace views {
class NativeWidgetViews;

namespace desktop {

class DesktopWindowView : public WidgetDelegateView,
                          public Widget::Observer {
 public:
  // The look and feel will be slightly different for different kinds of
  // desktop.
  enum DesktopType {
    DESKTOP_DEFAULT,
    DESKTOP_NETBOOK,
    DESKTOP_OTHER
  };

  static DesktopWindowView* desktop_window_view;

  DesktopWindowView(DesktopType type);
  virtual ~DesktopWindowView();

  static void CreateDesktopWindow(DesktopType type);

  // Changes activation to the specified Widget. The currently active Widget
  // is de-activated.
  void ActivateWidget(Widget* widget);

  NativeWidgetViews* active_native_widget() { return active_native_widget_; }

  void CreateTestWindow(const std::wstring& title,
                        SkColor color,
                        gfx::Rect initial_bounds,
                        bool rotate);

  DesktopType type() const { return type_; }

 private:
  // Overridden from View:
  virtual void Layout() OVERRIDE;

  // Overridden from WidgetDelegate:
  virtual Widget* GetWidget() OVERRIDE;
  virtual const Widget* GetWidget() const OVERRIDE;
  virtual bool CanResize() const OVERRIDE;
  virtual bool CanMaximize() const OVERRIDE;
  virtual std::wstring GetWindowTitle() const OVERRIDE;
  virtual SkBitmap GetWindowAppIcon() OVERRIDE;
  virtual SkBitmap GetWindowIcon() OVERRIDE;
  virtual bool ShouldShowWindowIcon() const OVERRIDE;
  virtual void WindowClosing() OVERRIDE;
  virtual View* GetContentsView() OVERRIDE;
  virtual NonClientFrameView* CreateNonClientFrameView() OVERRIDE;

  // Overridden from Widget::Observer.
  virtual void OnWidgetClosing(Widget* widget) OVERRIDE;
  virtual void OnWidgetVisibilityChanged(Widget* widget, bool visible) OVERRIDE;
  virtual void OnWidgetActivationChanged(Widget* widget, bool active) OVERRIDE;

  NativeWidgetViews* active_native_widget_;
  DesktopType type_;
  Widget* widget_;

  DISALLOW_COPY_AND_ASSIGN(DesktopWindowView);
};

}  // namespace desktop
}  // namespace views

#endif  // VIEWS_DESKTOP_DESKTOP_WINDOW_H_
