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
