// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBKIT_FILEAPI_SANDBOX_QUOTA_CLIENT_H_
#define WEBKIT_FILEAPI_SANDBOX_QUOTA_CLIENT_H_

#include <deque>
#include <list>
#include <map>
#include <string>

#include "base/basictypes.h"
#include "base/file_path.h"
#include "base/memory/scoped_ptr.h"
#include "webkit/fileapi/file_system_path_manager.h"
#include "webkit/fileapi/file_system_types.h"
#include "webkit/quota/quota_client.h"
#include "webkit/quota/quota_task.h"

namespace fileapi {

class FileSystemContext;

// An instance of this class is created per-profile.  This class
// is self-destructed and will delete itself when OnQuotaManagerDestroyed
// is called.
class SandboxQuotaClient : public quota::QuotaClient,
                           public quota::QuotaTaskObserver {
 public:
  SandboxQuotaClient(
      scoped_refptr<base::MessageLoopProxy> file_message_loop,
      FileSystemContext* file_system_context,
      bool is_incognito);
  virtual ~SandboxQuotaClient();

  // QuotaClient methods.
  virtual quota::QuotaClient::ID id() const OVERRIDE;
  virtual void OnQuotaManagerDestroyed() OVERRIDE;
  virtual void GetOriginUsage(const GURL& origin_url,
                              quota::StorageType type,
                              GetUsageCallback* callback) OVERRIDE;
  virtual void GetOriginsForType(quota::StorageType type,
                                 GetOriginsCallback* callback) OVERRIDE;
  virtual void GetOriginsForHost(quota::StorageType type,
                                 const std::string& host,
                                 GetOriginsCallback* callback) OVERRIDE;

 private:
  class GetOriginUsageTask;
  class GetOriginsTaskBase;
  class GetOriginsForTypeTask;
  class GetOriginsForHostTask;

  typedef std::pair<fileapi::FileSystemType, std::string> TypeAndHostOrOrigin;
  typedef quota::CallbackQueueMap1<GetUsageCallback*,
                                   TypeAndHostOrOrigin,
                                   int64
                                   > UsageCallbackMap;
  typedef quota::CallbackQueueMap1<GetOriginsCallback*,
                                   fileapi::FileSystemType,
                                   const std::set<GURL>&
                                   > OriginsForTypeCallbackMap;
  typedef quota::CallbackQueueMap1<GetOriginsCallback*,
                                   TypeAndHostOrOrigin,
                                   const std::set<GURL>&
                                   > OriginsForHostCallbackMap;

  void DidGetOriginUsage(fileapi::FileSystemType type,
                         const GURL& origin, int64 usage);
  void DidGetOriginsForType(fileapi::FileSystemType type,
                            const std::set<GURL>& origins);
  void DidGetOriginsForHost(const TypeAndHostOrOrigin& type_and_host,
                            const std::set<GURL>& origins);

  scoped_refptr<base::MessageLoopProxy> file_message_loop_;
  scoped_refptr<FileSystemContext> file_system_context_;

  bool is_incognito_;

  std::set<GURL> visited_origins_;

  // Pending callbacks.
  UsageCallbackMap pending_usage_callbacks_;
  OriginsForTypeCallbackMap pending_origins_for_type_callbacks_;
  OriginsForHostCallbackMap pending_origins_for_host_callbacks_;

  DISALLOW_COPY_AND_ASSIGN(SandboxQuotaClient);
};

}  // namespace fileapi

#endif  // WEBKIT_FILEAPI_SANDBOX_QUOTA_CLIENT_H_
