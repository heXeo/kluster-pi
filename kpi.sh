#!/usr/bin/env bash

path() {
  local filename=$(readlink "$1")
  [[ "$(uname -s)" == "Linux" ]] && {
    filename=$(readlink -f "$1")
  }
  [[ -z "$filename" ]] && {
    filename="$1"
  }
  local dirname=$(dirname "$filename")
  echo "$dirname"
}

SCRIPT_PATH=$(path "$0")

. $SCRIPT_PATH/libs/error_code.inc
. $SCRIPT_PATH/libs/logger.inc
. $SCRIPT_PATH/libs/utils.inc
. $SCRIPT_PATH/libs/core.inc
. $SCRIPT_PATH/libs/shell.inc

. $SCRIPT_PATH/cmds/root.inc
. $SCRIPT_PATH/cmds/bootcode.inc
. $SCRIPT_PATH/cmds/image.inc
. $SCRIPT_PATH/cmds/node.inc

# Script starts here !

DEBUG="${DEBUG:-0}"
KPI_BASE_DIR=/var/lib/kpi

trap 'on_exit' EXIT
trap 'on_error' ERR

function on_exit() {
  return
}

function on_error() {
  return
}

function main() {
  history -r "$SCRIPT_PATH/.history"
  while true
  do
    kpish_read
    history -s "$REPLY"
    kpish_handle "${KPISH_ARGS[@]}"
    history -w "$SCRIPT_PATH/.history"
  done
  exit 1
}

mount_nodes
main $@
