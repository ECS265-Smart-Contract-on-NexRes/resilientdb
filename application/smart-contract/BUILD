package(default_visibility = ["//visibility:public"])

cc_library(
    name = "kv_client",
    srcs = ["kv_client.cpp"],
    hdrs = ["kv_client.h"],
    deps = [
        "//client:resdb_user_client",
        "//proto:kv_server_cc_proto",
    ],
)

cc_binary(
    name = "smart_contract_client",
    srcs = ["smart_contract_client.cpp"],
    deps = [
        ":kv_client",
        "//config:resdb_config_utils",
    ],
)
