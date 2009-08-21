/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebKit_h
#define WebKit_h

#include "WebCommon.h"
#include "WebURL.h"

namespace v8 {
    class Extension;
}

namespace WebKit {
    class WebKitClient;
    class WebString;

    // Must be called on the thread that will be the main WebKit thread before
    // using any other WebKit APIs.  The provided WebKitClient must be non-null
    // and must remain valid until the current thread calls shutdown.
    WEBKIT_API void initialize(WebKitClient*);

    // Once shutdown, the WebKitClient passed to initialize will no longer be
    // accessed.  No other WebKit objects should be in use when this function
    // is called.  Any background threads created by WebKit are promised to be
    // terminated by the time this function returns.
    WEBKIT_API void shutdown();

    // Returns the WebKitClient instance passed to initialize.
    WEBKIT_API WebKitClient* webKitClient();

    // Alters the rendering of content to conform to a fixed set of rules.
    WEBKIT_API void setLayoutTestMode(bool);
    WEBKIT_API bool layoutTestMode();

    // Registers a URL scheme to be treated as a local scheme (i.e., with the
    // same security rules as those applied to "file" URLs).  This means that
    // normal pages cannot link to or access URLs of this scheme.
    WEBKIT_API void registerURLSchemeAsLocal(const WebString&);

    // Registers a URL scheme to be treated as a noAccess scheme.  This means
    // that pages loaded with this URL scheme cannot access pages loaded with
    // any other URL scheme.
    WEBKIT_API void registerURLSchemeAsNoAccess(const WebString&);

    // Registers a v8 extension to be available on webpages. The three forms
    // offer various restrictions on what types of contexts the extension is
    // loaded into. If a scheme is provided, only pages whose URL has the given
    // scheme will match. If extensionGroup is provided, the extension will only
    // be loaded into scripts run via WebFrame::ExecuteInNewWorld with the
    // matching group.
    // Will only affect v8 contexts initialized after this call. Takes ownership
    // of the v8::Extension object passed.
    WEBKIT_API void registerExtension(v8::Extension*);
    WEBKIT_API void registerExtension(v8::Extension*,
                                      const WebString& schemeRestriction);
    WEBKIT_API void registerExtension(v8::Extension*, int extensionGroup);

    // Enables special settings which are only applicable if V8 is executed
    // in the single thread which must be the main thread.
    // FIXME: make a try to dynamically detect when this condition is broken
    // and automatically switch off single thread mode.
    WEBKIT_API void enableV8SingleThreadMode();

    // Process any pending JavaScript console messages.
    WEBKIT_API void flushConsoleMessages();

    // Enables HTML5 media support.
    WEBKIT_API void enableMediaPlayer();

    // Purge the plugin list cache.
    WEBKIT_API void resetPluginCache();

    // Enables HTML5 database support.
    WEBKIT_API void enableDatabases();
    WEBKIT_API bool databasesEnabled();

    // Support for whitelisting access to origins beyond the same-origin policy.
    WEBKIT_API void whiteListAccessFromOrigin(
        const WebURL& sourceOrigin, const WebString& destinationProtocol,
        const WebString& destinationHost, bool allowDestinationSubdomains);
    WEBKIT_API void resetOriginAccessWhiteLists();

} // namespace WebKit

#endif
