. $SCRIPT_PATH/libs/utils.inc

function bootcode_get() {
  return 0
}

function bootcode_rm() {
  return 0
}

function bootcode_ls() {
  return 0
}

function bootcode_set() {
  return 0
}

function image_get() {
  [[ -z "$1" ]] && return 1

  local url="$1"
  local tmpfile=$(mktemp)
  
  curl_download "$url" "$tmpfile"
  [[ "$?" != "0" ]] && {
    return 1
  }

  image_extract "$tmpfile"
  [[ "$?" != "0" ]] && {
    return $(($? + 1))
  }

  return 0
}

function image_extract() {
  [[ -z "$1" ]] && return 1

  local archive_path="$1"
  local images_path="$KPI_BASE_DIR/images"
  local type=$(file -b --mime-type "$archive_path")

  [[ "$type" != "application/x-7z-compressed" ]] && {
    return 1
  }

  local image_id=$(image_next_id)
  mkdir -p "$images_path/$image_id/"
  7z x -so $tmpfile | tar xf - -C "$images_path/$image_id/"

  [[ "$?" != "0" ]] && {
    return 2
  }

  return 0
}

function image_next_id() {
  local images_path="$KPI_BASE_DIR/images"
  local current_id_path="$images_path/current_id"
  [[ ! -f "$current_id_path" ]] && {
    mkdir -p "$image_path"
    echo "0" > "$current_id_path"
  }
  local current_id=$(<"$current_id_path")
  local next_id=$(($current_id + 1))
  echo "$next_id" > "$current_id_path"
  echo "$next_id"
  return 0
}

function image_rm() {
  [[ -z "$1" ]] && return 1

  local image_id="$1"
  local images_path="$KPI_BASE_DIR/images"
  [[ -d "$images_path/$image_id" ]] && {
    rm -rf "$images_path/$image_id"
  }
  return 0
}

function image_ls() {
  local -n images_ref=$1
  local images_path="$KPI_BASE_DIR/images"

  [[ ! -d "$images_path" ]] || [[ -z $(ls -A "$images_path") ]] && return 0

  for path in $images_path/*
  do
    [[ ! -d "$path" ]] && continue
    local image_id=$(basename $path)
    [[ ! -f "$path/name" ]] && {
      touch "$path/name"
    }
    local image_name=$(<"$path/name")
    images_ref+=("$image_id:$image_name")
  done
  return 0
}

function image_rename() {
  [[ -z "$1" || -z "$2" ]] && return 1

  local image_id="$1"
  local image_name="$2"
  local images_path="$KPI_BASE_DIR/images"
  [[ -f "$images_path/$image_id/name" ]] && {
    echo "$image_name" > "$images_path/$image_id/name"
    return 0
  }
  return 1
}

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

function node_create() {
  return 0
}

function node_attach() {
  [[ -z "$1" || -z "$2" ]] && return 1

  local node_id="$1"
  local serial="$2"
  local nodes_path="$KPI_BASE_DIR/nodes"
  local tftp_path="$KPI_BASE_DIR/tftp"
  local nfs_path="$KPI_BASE_DIR/nfs"

  [[ ! -d "$nodes_path/$node_id" ]] && return 1
  [[ ! -L "$nodes_path/$node_id/image" ]] && return 1

  node_detach "$node_id"
  echo "$serial" > "$nodes_path/$node_id/serial"

  mkdir -p "$tftp_path"
  ln -s "$nodes_path/$node_id/bootfs" "$tftp_path/$serial"

  mkdir -p "$nfs_path/$node_id"
  ln -s "$nodes_path/$node_id/rootfs" "$nfs_path/$node_id/rootfs"
  ln -s "$nodes_path/$node_id/bootfs" "$nfs_path/$node_id/bootfs"

  return 0
}

function node_detach() {
  [[ -z "$1" ]] && return 1

  local node_id="$1"
  local nodes_path="$KPI_BASE_DIR/nodes"
  local tftp_path="$KPI_BASE_DIR/tftp"
  local nfs_path="$KPI_BASE_DIR/nfs"
  [[ -f "$nodes_path/$node_id/serial" ]] && {
    local serial=$(<"$nodes_path/$node_id/serial")
    rm "$tftp_path/$serial"
    rm "$nodes_path/$node_id/serial"
  }
  [[ -d "$nfs_path/$node_id" ]] && {
    rm -rf "$nfs_path/$node_id"
  }
  return 0
}

function node_rm() {
  [[ -z "$1" ]] && return 1

  local node_id="$1"
  local nodes_path="$KPI_BASE_DIR/nodes"
  [[ -d "$nodes_path/$node_id" ]] && {
    node_detach "$node_id"
    rm -rf "$nodes_path/$node_id"
  }
  return 0
}

function node_ls() {
  local -n nodes_ref=$1
  local nodes_path="$KPI_BASE_DIR/nodes"

  [[ ! -d "$nodes_path" ]] || [[ -z $(ls -A "$nodes_path") ]] && return 0

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
  return 0
}

function node_rename() {
  [[ -z "$1" || -z "$2" ]] && return 1

  local node_id="$1"
  local node_name="$2"
  local nodes_path="$KPI_BASE_DIR/nodes"
  [[ -f "$nodes_path/$node_id/name" ]] && {
    echo "$node_name" > "$nodes_path/$node_id/name"
    return 0
  }
  return 1
}

function node_setimg() {
  [[ -z "$1" || -z "$2" ]] && return 1

  local node_id="$1"
  local image_id="$2"
  local nodes_path="$KPI_BASE_DIR/nodes"
  local images_path="$KPI_BASE_DIR/images"
  [[ -d "$nodes_path/$node_id" && -d "$images_path/$image_id" ]] && {
    ln -s "$images_path/$image_id" "$nodes_path/$node_id/image"
    return 0
  }
  return 1
}
