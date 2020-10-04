# Join relative/absolute path with a relative path
function path_join() {
  shopt -s extglob
  echo "${1%%+(/)}${1:+/}$2"
  shopt -u extglob
}

function stack_trace() {
  while caller $((n++)); do :; done;
}

function fn_exists() {
  local t=$(LC_ALL=C type -t $1)
  [[ "$t" == "function" ]] && return 0 || return 1
}

function wget_download() {
  local url="$1"
  local output="$2"
  wget -qO "$output" "$url"
}

function curl_download() {
  local url="$1"
  local output="$2"
  curl -sL -o "$output" "$url"
}

function nfs_export {
  [[ -z "$1" || -z "$2" ]] && return 1

  local fs_id="$1"
  local fs_path="$2"
  local exports_path="/etc/exports.d/$fs_id.exports"

  [[ -f "$exports_path" ]] && return 1

  echo "$fs_path *(fsid=$fs_id,rw,sync,no_subtree_check,no_root_squash)" > $exports_path
  exportfs -r

  return 0
}

function nfs_unexport {
  [[ -z "$1" ]] && return 1

  local fs_id="$1"
  local exports_path="/etc/exports.d/$fs_id.exports"

  [[ ! -f "$exports_path" ]] && return 1

  rm $exports_path
  exportfs -r

  return 0
}
