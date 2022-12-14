#include "kv_server/db_client.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace resdb {

DBClient::DBClient(const std::string& ip, int port) {
  sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  assert(inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr) >= 0);

  assert(connect(sockfd_, (struct sockaddr*)&servaddr, sizeof(servaddr)) >= 0);
}

std::string DBClient::QueryDB(const std::string& data) {
  if (write(sockfd_, (void*)data.c_str(), data.size()) != data.size()) {
    return "";
  }
  char buf[1024];
  int ret = read(sockfd_, buf, sizeof(buf));
  return std::string(buf, ret);
}
}  // namespace resdb
