. $SCRIPT_PATH/libs/colors.inc
. $SCRIPT_PATH/libs/core.inc

# Events handlers

function on_node_created() {
  return 0
}

function on_node_renamed() {
  local node_id="$1"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"
  local node_name=$(<"$node_path/name")
  
  findmnt -M "$node_path/rootfs" > /dev/null 2>&1
  [[ "$?" == "1" ]] && return 0

  echo "$node_name" > "$node_path/rootfs/etc/hostname"

  return 0
}

function on_node_deleted() {
  return 0
}

function on_node_rootfs_mount() {
  return 0
}

function on_node_rootfs_mounted() {
  local node_id="$1"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"
  local node_name=$(<"$node_path/name")

  nfs_export "$node_id" "$node_path/rootfs"

  findmnt -M "$node_path/rootfs" > /dev/null 2>&1
  [[ "$?" == "1" ]] && return 0

  echo "$node_name" > "$node_path/rootfs/etc/hostname"
  
  return 0
}

function on_node_rootfs_unmount() {
  local node_id="$1"
  nfs_unexport "$node_id"
  return 0
}

function on_node_rootfs_unmounted() {
  return 0
}

function on_node_bootfs_mount() {
  return 0
}

function on_node_bootfs_mounted() {
  return 0
}

function on_node_bootfs_unmount() {
  return 0
}

function on_node_bootfs_unmounted() {
  return 0
}

# Commands handlers

function kpish_node_commands() {
  echo "create rm ls rename setimg attach detach"
}

function kpish_node_help() {
  log "create <name> [img_id]"
  log "rm <id>"
  log "ls"
  log "rename <id> <name>"
  log "setimg <id> <image_id>"
  log "attach <id> <serial>"
  log "detach <id>"
  log "options"
}

function kpish_node_create_handler() {
  node_create "$2" "$3"
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
