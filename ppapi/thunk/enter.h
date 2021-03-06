// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PPAPI_THUNK_ENTER_H_
#define PPAPI_THUNK_ENTER_H_

#include "base/basictypes.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/shared_impl/api_id.h"
#include "ppapi/shared_impl/ppapi_globals.h"
#include "ppapi/shared_impl/proxy_lock.h"
#include "ppapi/shared_impl/resource.h"
#include "ppapi/shared_impl/resource_tracker.h"
#include "ppapi/thunk/ppapi_thunk_export.h"
#include "ppapi/thunk/ppb_instance_api.h"
#include "ppapi/thunk/resource_creation_api.h"

namespace ppapi {
namespace thunk {

// Enter* helper objects: These objects wrap a call from the C PPAPI into
// the internal implementation. They make sure the lock is acquired and will
// automatically set up some stuff for you.
//
// You should always check whether the enter succeeded before using the object.
// If this fails, then the instance or resource ID supplied was invalid.
//
// The |report_error| arguments to the constructor should indicate if errors
// should be logged to the console. If the calling function expects that the
// input values are correct (the normal case), this should be set to true. In
// some case like |IsFoo(PP_Resource)| the caller is questioning whether their
// handle is this type, and we don't want to report an error if it's not.
//
// Resource member functions: EnterResource
//   Automatically interprets the given PP_Resource as a resource ID and sets
//   up the resource object for you.

namespace subtle {

// This helps us define our RAII Enter classes easily. To make an RAII class
// which locks the proxy lock on construction and unlocks on destruction,
// inherit from |LockOnEntry<true>|. For cases where you don't want to lock,
// inherit from |LockOnEntry<false>|. This allows us to share more code between
// Enter* and Enter*NoLock classes.
template <bool lock_on_entry>
struct LockOnEntry;

template <>
struct LockOnEntry<false> {
// TODO(dmichael) assert the lock is held.
};

template <>
struct LockOnEntry<true> {
  LockOnEntry() {
    ppapi::ProxyLock::Acquire();
  }
  ~LockOnEntry() {
    ppapi::ProxyLock::Release();
  }
};

// Keep non-templatized since we need non-inline functions here.
class PPAPI_THUNK_EXPORT EnterBase {
 public:
  EnterBase();
  explicit EnterBase(const PP_CompletionCallback& callback);
  virtual ~EnterBase();

  // Sets the result.
  //
  // Returns the "retval()". This is to support the typical usage of
  //   return enter.SetResult(...);
  // without having to write a separate "return enter.retval();" line.
  int32_t SetResult(int32_t result);

  // Use this value as the return value for the function.
  int32_t retval() const { return retval_; }

 protected:
  // Helper function to return a Resource from a PP_Resource. Having this
  // code be in the non-templatized base keeps us from having to instantiate
  // it in every template.
  Resource* GetResource(PP_Resource resource) const;

  // Does error handling associated with entering a resource. The resource_base
  // is the result of looking up the given pp_resource. The object is the
  // result of converting the base to the desired object (converted back to a
  // Resource* so this function doesn't have to be templatized). The reason for
  // passing both resource_base and object is that we can differentiate "bad
  // resource ID" from "valid resource ID not of the currect type."
  //
  // This will set retval_ = PP_ERROR_BADRESOURCE if the object is invalid, and
  // if report_error is set, log a message to the programmer.
  void SetStateForResourceError(PP_Resource pp_resource,
                                Resource* resource_base,
                                void* object,
                                bool report_error);

  // Same as SetStateForResourceError except for function API.
  void SetStateForFunctionError(PP_Instance pp_instance,
                                void* object,
                                bool report_error);

 private:
  // Holds the callback. The function will only be non-NULL when the
  // callback is requried. Optional callbacks don't require any special
  // handling from us at this layer.
  PP_CompletionCallback callback_;

  int32_t retval_;
};

}  // namespace subtle

// EnterResource ---------------------------------------------------------------

template<typename ResourceT, bool lock_on_entry = true>
class EnterResource : public subtle::EnterBase,
                      public subtle::LockOnEntry<lock_on_entry> {
 public:
  EnterResource(PP_Resource resource, bool report_error)
      : EnterBase() {
    Init(resource, report_error);
  }
  EnterResource(PP_Resource resource,
                const PP_CompletionCallback& callback,
                bool report_error)
      : EnterBase(callback) {
    Init(resource, report_error);
  }
  ~EnterResource() {}

  bool succeeded() const { return !!object_; }
  bool failed() const { return !object_; }

  ResourceT* object() { return object_; }
  Resource* resource() { return resource_; }

 private:
  void Init(PP_Resource resource, bool report_error) {
    resource_ = GetResource(resource);
    if (resource_)
      object_ = resource_->GetAs<ResourceT>();
    else
      object_ = NULL;
    SetStateForResourceError(resource, resource_, object_, report_error);
  }

  Resource* resource_;
  ResourceT* object_;

  DISALLOW_COPY_AND_ASSIGN(EnterResource);
};

// ----------------------------------------------------------------------------

// Like EnterResource but assumes the lock is already held.
template<typename ResourceT>
class EnterResourceNoLock : public EnterResource<ResourceT, false> {
 public:
  EnterResourceNoLock(PP_Resource resource, bool report_error)
      : EnterResource<ResourceT, false>(resource, report_error) {
  }
};

// EnterInstance ---------------------------------------------------------------

class PPAPI_THUNK_EXPORT EnterInstance
    : public subtle::EnterBase,
      public subtle::LockOnEntry<true> {
 public:
  EnterInstance(PP_Instance instance);
  EnterInstance(PP_Instance instance,
                const PP_CompletionCallback& callback);
  ~EnterInstance();

  bool succeeded() const { return !!functions_; }
  bool failed() const { return !functions_; }

  PPB_Instance_API* functions() { return functions_; }

 private:
  PPB_Instance_API* functions_;
};

class PPAPI_THUNK_EXPORT EnterInstanceNoLock
    : public subtle::EnterBase,
      public subtle::LockOnEntry<false> {
 public:
  EnterInstanceNoLock(PP_Instance instance);
  //EnterInstanceNoLock(PP_Instance instance,
  //                    const PP_CompletionCallback& callback);
  ~EnterInstanceNoLock();

  bool succeeded() const { return !!functions_; }
  bool failed() const { return !functions_; }

  PPB_Instance_API* functions() { return functions_; }

 private:
  PPB_Instance_API* functions_;
};

// EnterResourceCreation -------------------------------------------------------

class PPAPI_THUNK_EXPORT EnterResourceCreation
    : public subtle::EnterBase,
      public subtle::LockOnEntry<true> {
 public:
  EnterResourceCreation(PP_Instance instance);
  ~EnterResourceCreation();

  bool succeeded() const { return !!functions_; }
  bool failed() const { return !functions_; }

  ResourceCreationAPI* functions() { return functions_; }

 private:
  ResourceCreationAPI* functions_;
};

class PPAPI_THUNK_EXPORT EnterResourceCreationNoLock
    : public subtle::EnterBase,
      public subtle::LockOnEntry<false> {
 public:
  EnterResourceCreationNoLock(PP_Instance instance);
  ~EnterResourceCreationNoLock();

  bool succeeded() const { return !!functions_; }
  bool failed() const { return !functions_; }

  ResourceCreationAPI* functions() { return functions_; }

 private:
  ResourceCreationAPI* functions_;
};

}  // namespace thunk
}  // namespace ppapi

#endif  // PPAPI_THUNK_ENTER_H_
