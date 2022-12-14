#include "kv_server/kv_server_executor.h"
#include "kv_server/db_client.h"

#include <glog/logging.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "proto/kv_server.pb.h"

namespace resdb {

KVServerExecutor::KVServerExecutor(const ResConfigData& config_data,
                                   char* cert_file)
    : l_storage_layer_(cert_file, config_data),
      r_storage_layer_(cert_file, config_data) {
  equip_rocksdb_ = config_data.rocksdb_info().enable_rocksdb();
  equip_leveldb_ = config_data.leveldb_info().enable_leveldb();
}

KVServerExecutor::KVServerExecutor(void) {}

std::unique_ptr<std::string> KVServerExecutor::ExecuteData(
    const std::string& request) {
  KVRequest kv_request;

  DBClient client("127.0.0.1",6000);
  std::unique_ptr<std::string> resp_str = std::make_unique<std::string>();
  *resp_str = client.QueryDB(kv_request.value());
  return resp_str;
}

}  // namespace resdb
