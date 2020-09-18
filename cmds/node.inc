. $SCRIPT_PATH/libs/colors.inc
. $SCRIPT_PATH/libs/core.inc

function kpish_node_commands() {
  echo "create rm ls rename setimg attach detach"
}

function kpish_node_ls_handler() {
  local nodes=()
  node_ls nodes
  
  log "${LIGHT_WHITE}id\timg id\tserial\tname${BLANK}"

  for node in "${nodes[@]}"
  do
    local node_id=${node%%:*}
    local node_img_id=${node#*:}
    node_img_id=${node_img_id%%:*}
    local node_serial=${node%:*}
    node_serial=${node_serial##*:}
    local node_name=${node##*:}
    log "$node_id\t$node_img_id\t$node_serial\t$node_name"
  done
}

function kpish_node_rename_handler() {
  node_rename "$2" "$3"
}

function kpish_node_rm_handler() {
  node_rm "$2"
}

function kpish_node_setimg_handler() {
  node_setimg "$2" "$3"
}

function kpish_node_attach_handler() {
  node_attach "$2" "$3"
}

function kpish_node_detach_handler() {
  node_detach "$2"
}
