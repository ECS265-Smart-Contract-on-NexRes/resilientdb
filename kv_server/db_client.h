#pragma once

#include <string>

namespace resdb {

class DBClient {
 public:
  DBClient(const std::string& ip, int port);
  std::string QueryDB(const std::string&);

 private:
  int sockfd_;
};

}  // namespace resdb
