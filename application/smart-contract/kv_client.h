#pragma once

#include "client/resdb_user_client.h"

namespace resdb {

// KVClient to send data to the kv server.
class KVClient : public ResDBUserClient {
 public:
  KVClient(const ResDBConfig& config);

  int Set(const std::string& key, const std::string& data);
  std::unique_ptr<std::string> Get(const std::string& key);
  std::unique_ptr<std::string> GetValues();
};

}  // namespace resdb
