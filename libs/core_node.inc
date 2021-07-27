function node_next_id() {
  local nodes_path="$KPI_BASE_DIR/nodes"
  local current_id_path="$nodes_path/current_id"
  [[ ! -f "$current_id_path" ]] && {
    mkdir -p "$nodes_path"
    echo "0" > "$current_id_path"
  }
  local current_id=$(<"$current_id_path")
  local next_id=$(($current_id + 1))
  echo "$next_id" > "$current_id_path"
  echo "$next_id"
  return 0
}

function node_ls() {
  local -n nodes_ref=$1
  local nodes_path="$KPI_BASE_DIR/nodes"

  [[ ! -d "$nodes_path" ]] || [[ -z $(ls -A "$nodes_path") ]] && return $R_OK

  for path in $nodes_path/*
  do
    [[ ! -d "$path" ]] && continue
    local node_id=$(basename $path)
    local node_name=$(<"$path/name")
    local node_img_id=""
    [[ -f "$path/image/id" ]] && {
      node_img_id=$(<"$path/image/id")
    }
    local node_serial=""
    [[ -f "$path/serial" ]] && {
      node_serial=$(<"$path/serial")
    }
    nodes_ref+=("$node_id:$node_img_id:$node_serial:$node_name")
  done
  return $R_OK
}

function node_create() {
  [[ -z "$1" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_name="$1"
  local image_id="$2"
  local node_id=$(node_next_id)
  local node_path="$KPI_BASE_DIR/nodes/$node_id"

  mkdir -p "$node_path"
  mkdir -p "$node_path/rootfs_rw"
  mkdir -p "$node_path/rootfs_work"
  mkdir -p "$node_path/rootfs"

  ln -s "$node_path/image/rootfs" "$node_path/rootfs_ro"

  echo "$node_id" > "$node_path/id"
  echo "$node_name" > "$node_path/name"

  [[ -n "$image_id" ]] && node_setimg "$node_id" "$image_id"

  on_node_created "$node_id"

  return $R_OK
}

function node_rename() {
  [[ -z "$1" || -z "$2" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local node_name="$2"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND

  echo "$node_name" > "$node_path/name"

  on_node_renamed "$node_id"

  return $R_OK
}

function node_setimg() {
  [[ -z "$1" || -z "$2" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local image_id="$2"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"
  local image_path="$KPI_BASE_DIR/images/$image_id"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND
  [[ ! -d "$image_path" ]] && return $R_IMAGE_NOT_FOUND

  node_unmount_rootfs "$node_id"
  rm "$node_path/image" > /dev/null 2>&1
  rm -rf "$node_path/rootfs_rw/*"
  rm -rf "$node_path/rootfs_work/*"
  rm -rf "$node_path/rootfs/*"
  ln -s "$image_path" "$node_path/image"
  cp -a "$image_path/bootfs" "$node_path/bootfs"
  node_mount_rootfs "$node_id"

  return $R_OK
}

function node_attach() {
  [[ -z "$1" || -z "$2" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local node_serial="$2"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"
  local tftp_path="$KPI_BASE_DIR/tftp"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND

  node_unmount_bootfs "$node_id"
  echo "$node_serial" > "$node_path/serial"
  node_mount_bootfs "$node_id"

  return $R_OK
}

function node_detach() {
  [[ -z "$1" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"
  
  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND

  node_unmount_bootfs "$node_id"
  rm "$node_path/serial"

  return $R_OK
}

function node_mount() {
  [[ -z "$1" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND

  node_mount_rootfs "$node_id"
  node_mount_bootfs "$node_id"

  return $R_OK
}

function node_unmount() {
  [[ -z "$1" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND

  node_unmount_rootfs "$node_id"
  node_unmount_bootfs "$node_id"

  return $R_OK
}

function node_rm() {
  [[ -z "$1" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND

  node_unmount "$node_id"
  rm -rf "$node_path"

  on_node_deleted "$node_id"

  return $R_OK
}

function node_mount_rootfs() {
  [[ -z "$1" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"

  [[ ! -d "$node_path" || ! -d "$node_path/rootfs_ro" ]] && return $R_NODE_NOT_FOUND

  findmnt -M "$node_path/rootfs" > /dev/null 2>&1
  [[ "$?" == "0" ]] && return $R_MOUNT_ERROR

  on_node_rootfs_mount "$node_id"
  mount -t overlay overlay -o lowerdir="$node_path/rootfs_ro",upperdir="$node_path/rootfs_rw",workdir="$node_path/rootfs_work" "$node_path/rootfs" -o index=on -o nfs_export=on
  on_node_rootfs_mounted "$node_id"

  return $R_OK
}

function node_unmount_rootfs() {
  [[ -z "$1" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND

  findmnt -M "$node_path/rootfs" > /dev/null 2>&1
  [[ "$?" == "1" ]] && return $R_MOUNT_ERROR

  on_node_rootfs_unmount "$node_id"
  umount "$node_path/rootfs"
  on_node_rootfs_unmounted "$node_id"

  return $R_OK
}

function node_mount_bootfs() {
  [[ -z "$1" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"
  local tftp_path="$KPI_BASE_DIR/tftp"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND
  [[ ! -f "$node_path/serial" ]] && return $R_NODE_NOT_FOUND

  local node_serial=$(<"$node_path/serial")

  [[ -z "$node_serial" ]] && return $R_NOK
  [[ -L "$tftp_path/$node_serial" ]] && return $R_NOK

  on_node_bootfs_mount "$node_id"
  ln -s "$node_path/bootfs" "$tftp_path/$node_serial"
  on_node_bootfs_mounted "$node_id"

  return $R_OK
}

function node_unmount_bootfs() {
  [[ -z "$1" ]] && return $R_TOO_FEW_ARGUMENTS

  local node_id="$1"
  local node_path="$KPI_BASE_DIR/nodes/$node_id"
  local tftp_path="$KPI_BASE_DIR/tftp"

  [[ ! -d "$node_path" ]] && return $R_NODE_NOT_FOUND
  [[ ! -f "$node_path/serial" ]] && return $R_NODE_NOT_FOUND

  local node_serial=$(<"$node_path/serial")

  [[ -z "$node_serial" ]] && return $R_NOK
  [[ ! -L "$tftp_path/$node_serial" ]] && return $R_NOK

  on_node_bootfs_unmount "$node_id"
  rm "$tftp_path/$node_serial"
  on_node_bootfs_unmounted "$node_id"

  return $R_OK
}

function mount_nodes() {
  local nodes_path="$KPI_BASE_DIR/nodes"

  [[ ! -d "$nodes_path" ]] || [[ -z $(ls -A "$nodes_path") ]] && return $R_OK

  for path in $nodes_path/*
  do
    [[ ! -d "$path" ]] && continue
    local node_id=$(basename $path)
    node_mount "$node_id"
  done

  return $R_OK
}

function unmount_nodes() {
  local nodes_path="$KPI_BASE_DIR/nodes"

  [[ ! -d "$nodes_path" ]] || [[ -z $(ls -A "$nodes_path") ]] && return $R_OK

  for path in $nodes_path/*
  do
    [[ ! -d "$path" ]] && continue
    local node_id=$(basename $path)
    node_unmount "$node_id"
  done

  return $R_OK
}
