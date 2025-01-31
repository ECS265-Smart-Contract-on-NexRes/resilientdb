#include "sdk_client/pistache_service.h"

#include <glog/logging.h>

#include <fstream>
#include <memory>
#include <string>
#include <thread>

// using resdb::GenerateReplicaInfo;
// using resdb::ReplicaInfo;
// using resdb::ResDBKVClient;

namespace resdb {

void PistacheService::configureRoutes() {
  Pistache::Rest::Routes::Get(
      router_, "/v1/transactions/:txid?",
      Pistache::Rest::Routes::bind(&PistacheService::getAssets, this));
  Pistache::Rest::Routes::Post(
      router_, "/v1/transactions/commit",
      Pistache::Rest::Routes::bind(&PistacheService::commit, this));
}

void PistacheService::commit(const Request& request, Response response) {
  try {
    const std::string json = request.body();
    resdb::SDKTransaction transaction = resdb::fromJson(json);

    const std::string id = transaction.id;
    const std::string value = transaction.value;

    if (value == "") {  // invalid transaction
      LOG(ERROR) << "Invalid asset format in commit request";
      response.send(Pistache::Http::Code::Bad_Request, "Invalid asset format.",
                    MIME(Text, Plain));
      return;
    }
    ResDBKVClient client(config_);
    int ret = client.Set(id, value);
    LOG(INFO) << "client set ret = " << ret;
    if (ret != 0) {
      response.send(Pistache::Http::Code::Internal_Server_Error, "id: " + id,
                    MIME(Text, Plain));
    }
    LOG(INFO) << "Set " << id << " to " << value;

    response.send(Pistache::Http::Code::Created, "id: " + id,
                  MIME(Text, Plain));
  } catch (const std::runtime_error& bang) {
    response.send(Pistache::Http::Code::Not_Found, bang.what(),
                  MIME(Text, Plain));
  } catch (...) {
    response.send(Pistache::Http::Code::Internal_Server_Error, "Internal error",
                  MIME(Text, Plain));
  }
}
void PistacheService::getAssets(const Request& request, Response response) {
  try {
    if (!request.hasParam(":txid")) {
      ResDBKVClient client(config_);
      auto res = client.GetValues();
      if (res != nullptr) {
        LOG(INFO) << "client getvalues value = " << res->c_str();
      }
      response.send(Pistache::Http::Code::Ok, res->c_str(),
                    MIME(Application, Json));
      return;
    }

    std::string id = request.param(":txid").as<std::string>();

    ResDBKVClient client(config_);
    auto value = client.Get(id);
    if (value != nullptr) {
      LOG(INFO) << "client get value = " << value->c_str();
      response.send(Pistache::Http::Code::Ok, value->c_str(),
                    MIME(Application, Json));
    } else {
      LOG(ERROR) << "client get value fail for id " << id;
      response.send(Pistache::Http::Code::Internal_Server_Error,
                    "Get value fail", MIME(Text, Plain));
    }
  } catch (const std::runtime_error& bang) {
    response.send(Pistache::Http::Code::Not_Found, bang.what(),
                  MIME(Text, Plain));
  } catch (...) {
    response.send(Pistache::Http::Code::Internal_Server_Error, "Internal error",
                  MIME(Text, Plain));
  }
}

void PistacheService::run() {
  LOG(INFO) << "Starting on port " << portNum_ << " with " << numThreads_
            << " threads.";
  endpoint_->init(Pistache::Http::Endpoint::options().threads(numThreads_));
  configureRoutes();
  endpoint_->setHandler(router_.handler());
  endpoint_->serve();
}

}  // namespace resdb
