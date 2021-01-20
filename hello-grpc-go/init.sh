#!/bin/bash
cd "$(
  cd "$(dirname "$0")" >/dev/null 2>&1
  pwd -P
)/" || exit
export GO111MODULE="on"
export GOPROXY=https://mirrors.aliyun.com/goproxy/
go get github.com/golang/protobuf/protoc-gen-go
sh proto2go.sh
go list -mod=mod -json all
go build