#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <filesystem>

#include "config/resdb_config_utils.h"
#include "application/smart-contract/kv_client.h"

using resdb::GenerateResDBConfig;
using resdb::ResDBConfig;
using resdb::KVClient;

std::string GetFileData(const std::string& filename){
  int fd = open(filename.c_str(),O_RDONLY);
  if(fd<0) {
    printf("open file fail: %s\n",strerror(errno));
    return "";
  }
  
  std::string ret;

  char buf[8192];
  while(true){ 
    ssize_t sz = read(fd, buf, sizeof(buf));
    if(sz<0){
      printf("read data fail: %s\n",strerror(errno));
      ret = "";
      break;
    }
    if(sz==0){
      break;
    }
    ret += std::string(buf, sz);
  }
  close(fd);
  return ret;
}

std::string GetFileName(const std::string& filename) {
  return std::filesystem::path(filename).filename();
}

int main(int argc, char** argv) {
  if (argc < 3) {
    printf("<config path> <file path>\n");
    return 0;
  }
  std::string client_config_file = argv[1];
  std::string file = argv[2];

  ResDBConfig config = GenerateResDBConfig(client_config_file);
  config.SetClientTimeoutMs(100000);

  std::string file_data = GetFileData(file);

  KVClient client(config);
  int ret = client.Set(GetFileName(file), file_data);
  printf("set ret %d\n", ret);
  return ret>=0? 0:-1;
}
