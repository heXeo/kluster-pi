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
  mkdir -p "$images_path/$image_id"
  echo "$image_id" > "$images_path/$image_id/id"
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
    mkdir -p "$images_path"
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
