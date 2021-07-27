function node_option_add() {
  [[ -z "$1" || -z "$2" || -z "$3" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local option_key="$2"
  local option_value="$3"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND
  [[ ! -f "$node_path/options" ]] && touch "$node_path/options"

  grep -Eq "^${option_key}=" "$node_path/options" && {
    sed -Ei'' "s/^${option_key}=.*$/${option_key}=${option_value}/" "$node_path/options"
  } || {
    echo "$option_key=$option_value" >> "$node_path/options"
  }

  return $R_OK
}

function node_option_del() {
  [[ -z "$1" || -z "$2" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local option_key="$2"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND
  [[ ! -f "$node_path/options" ]] && return $R_OK

  sed -Ei'' "/^${option_key}=.*$/d" "$node_path/options"

  return $R_OK
}

function node_option_ls() {
  [[ -z "$1" ]] && return $R_TOO_FEW_ARGUMENTS

  local -n options_ref=$2
  local node_id="$1"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND
  [[ ! -f "$node_path/options" ]] && return $R_ON

  while IFS= read -r option
  do
    options_ref+=("$option")
  done <"$node_path/options"

  return $R_OK
}
