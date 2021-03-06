// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_CLIENT_REF_COUNTED_H_
#define GPU_COMMAND_BUFFER_CLIENT_REF_COUNTED_H_

#if defined(__native_client__)
#include "native_client/src/include/ref_counted.h"
namespace gpu {
template <class T>
struct RefCountedThreadSafe : public nacl::RefCounted<T> {
};
}
#else
#include "base/memory/ref_counted.h"
namespace gpu {
template <class T>
struct RefCountedThreadSafe : public base::RefCountedThreadSafe<T> {
};
}
#endif

#endif  // GPU_COMMAND_BUFFER_CLIENT_REF_COUNTED_H_
