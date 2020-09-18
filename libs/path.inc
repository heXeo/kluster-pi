# Join relative/absolute path with a relative path
function path_join() {
  shopt -s extglob
  echo "${1%%+(/)}${1:+/}$2"
  shopt -u extglob
}
