. $SCRIPT_PATH/libs/colors.inc
. $SCRIPT_PATH/libs/core.inc

# Events handlers

function node_event_handler() {
  local event_name="$1"
  local node_id="$2"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"

  [[ -x $node_path/events_handler ]] && {
    $node_path/events_handler "node_$1" "$node_path"
  }

  return 0
}

# Commands handlers

function node_exit_code_handler() {
  local exit_code="$1"
  [[ "$exit_code" -eq $R_NOK ]] && error "Unknown error."
  [[ "$exit_code" -eq $R_TOO_FEW_ARGUMENTS ]] && error "Too few arguments."
  [[ "$exit_code" -eq $R_NODE_NOT_FOUND ]] && error "Node not found."
  [[ "$exit_code" -eq $R_IMAGE_NOT_FOUND ]] && error "Image not found."
  return 0
}

function kpish_node_commands() {
  echo "create rm ls rename setimg attach detach options"
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
  node_exit_code_handler "$?" || return 1
  return 0
}

function kpish_node_ls_handler() {
  local nodes=()
  node_ls nodes
  node_exit_code_handler "$?" || return 1
  
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

  return 0
}

function kpish_node_rename_handler() {
  node_rename "$2" "$3"
  node_exit_code_handler "$?" || return 1
  return 0
}

function kpish_node_rm_handler() {
  node_rm "$2"
  node_exit_code_handler "$?" || return 1
  return 0
}

function kpish_node_setimg_handler() {
  node_setimg "$2" "$3"
  node_exit_code_handler "$?" || return 1
  return 0
}

function kpish_node_attach_handler() {
  node_attach "$2" "$3"
  node_exit_code_handler "$?" || return 1
  return 0
}

function kpish_node_detach_handler() {
  node_detach "$2"
  node_exit_code_handler "$?" || return 1
  return 0
}

function kpish_node_options_commands() {
  echo "add del show"
}

function kpish_node_options_help() {
  log "add <id> <key> <value>"
  log "del <id> <key>"
  log "show <id>"
}

function kpish_node_options_show_handler() {
  local options=()
  node_option_ls "$2" options
  node_exit_code_handler "$?" || return 1

  for option in "${options[@]}"
  do
    log "$option"
  done

  return 0
}

function kpish_node_options_add_handler() {
  node_option_add "$2" "$3" "$4"
  node_exit_code_handler "$?" || return 1
  return 0
}

function kpish_node_options_del_handler() {
  node_option_del "$2" "$3"
  node_exit_code_handler "$?" || return 1
  return 0
}


