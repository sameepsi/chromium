// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/shell_integration.h"

#include "base/message_loop.h"
#include "base/path_service.h"
#include "base/thread.h"
#include "chrome/browser/browser_process.h"
#include "chrome/common/chrome_paths.h"

///////////////////////////////////////////////////////////////////////////////
// ShellIntegration::DefaultBrowserWorker
//

ShellIntegration::DefaultBrowserWorker::DefaultBrowserWorker(
    DefaultBrowserObserver* observer)
    : observer_(observer),
      ui_loop_(MessageLoop::current()),
      file_loop_(g_browser_process->file_thread()->message_loop()) {
}

void ShellIntegration::DefaultBrowserWorker::StartCheckDefaultBrowser() {
  observer_->SetDefaultBrowserUIState(STATE_PROCESSING);
  file_loop_->PostTask(FROM_HERE, NewRunnableMethod(this,
      &DefaultBrowserWorker::ExecuteCheckDefaultBrowser));
}

void ShellIntegration::DefaultBrowserWorker::StartSetAsDefaultBrowser() {
  observer_->SetDefaultBrowserUIState(STATE_PROCESSING);
  file_loop_->PostTask(FROM_HERE, NewRunnableMethod(this,
      &DefaultBrowserWorker::ExecuteSetAsDefaultBrowser));
}

void ShellIntegration::DefaultBrowserWorker::ObserverDestroyed() {
  // Our associated view has gone away, so we shouldn't call back to it if
  // our worker thread returns after the view is dead.
  observer_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// DefaultBrowserWorker, private:

void ShellIntegration::DefaultBrowserWorker::ExecuteCheckDefaultBrowser() {
  DCHECK(MessageLoop::current() == file_loop_);
  DefaultBrowserState state = ShellIntegration::IsDefaultBrowser();
  ui_loop_->PostTask(FROM_HERE, NewRunnableMethod(this,
      &DefaultBrowserWorker::CompleteCheckDefaultBrowser, state));
}

void ShellIntegration::DefaultBrowserWorker::CompleteCheckDefaultBrowser(
    DefaultBrowserState state) {
  DCHECK(MessageLoop::current() == ui_loop_);
  UpdateUI(state);
}

void ShellIntegration::DefaultBrowserWorker::ExecuteSetAsDefaultBrowser() {
  DCHECK(MessageLoop::current() == file_loop_);
  ShellIntegration::SetAsDefaultBrowser();
  ui_loop_->PostTask(FROM_HERE, NewRunnableMethod(this,
      &DefaultBrowserWorker::CompleteSetAsDefaultBrowser));
}

void ShellIntegration::DefaultBrowserWorker::CompleteSetAsDefaultBrowser() {
  DCHECK(MessageLoop::current() == ui_loop_);
  if (observer_) {
    // Set as default completed, check again to make sure it stuck...
    StartCheckDefaultBrowser();
  }
}

void ShellIntegration::DefaultBrowserWorker::UpdateUI(
    DefaultBrowserState state) {
  if (observer_) {
    switch (state) {
      case NOT_DEFAULT_BROWSER:
        observer_->SetDefaultBrowserUIState(STATE_NOT_DEFAULT);
        break;
      case IS_DEFAULT_BROWSER:
        observer_->SetDefaultBrowserUIState(STATE_IS_DEFAULT);
        break;
      case UNKNOWN_DEFAULT_BROWSER:
        observer_->SetDefaultBrowserUIState(STATE_UNKNOWN);
        break;
      default:
        break;
    }
  }
}
