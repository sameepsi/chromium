// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REMOTING_PROTOCOL_MOCK_OBJECTS_H_
#define REMOTING_PROTOCOL_MOCK_OBJECTS_H_

#include "remoting/proto/internal.pb.h"
#include "remoting/protocol/connection_to_client.h"
#include "testing/gmock/include/gmock/gmock.h"

namespace remoting {
namespace protocol {

class ChromotocolConnection;

class MockConnectionToClient : public ConnectionToClient {
 public:
  MockConnectionToClient() {}

  MOCK_METHOD1(Init, void(ChromotocolConnection* connection));
  MOCK_METHOD2(SendInitClientMessage, void(int width, int height));
  MOCK_METHOD1(SendVideoPacket, void(const VideoPacket& packet));
  MOCK_METHOD0(GetPendingUpdateStreamMessages, int());
  MOCK_METHOD0(Disconnect, void());

 private:
  DISALLOW_COPY_AND_ASSIGN(MockConnectionToClient);
};

class MockConnectionToClientEventHandler :
  public ConnectionToClient::EventHandler {
 public:
  MockConnectionToClientEventHandler() {}

  MOCK_METHOD2(HandleMessage,
               void(ConnectionToClient* viewer,
                    ChromotingClientMessage* message));
  MOCK_METHOD1(OnConnectionOpened, void(ConnectionToClient* connection));
  MOCK_METHOD1(OnConnectionClosed, void(ConnectionToClient* connection));
  MOCK_METHOD1(OnConnectionFailed, void(ConnectionToClient* connection));

 private:
  DISALLOW_COPY_AND_ASSIGN(MockConnectionToClientEventHandler);
};

}  // namespace protocol
}  // namespace remoting

#endif  // REMOTING_PROTOCOL_MOCK_OBJECTS_H_
