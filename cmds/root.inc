function kpish_root_commands() {
  echo "bootcode image node version exit"
}

function kpish_root_version_handler() {
  log "1.0.0-rc.1"
  return 0
}

function kpish_root_exit_handler() {
  exit 0
}
