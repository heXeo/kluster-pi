. $SCRIPT_PATH/libs/colors.inc

function log() {
  echo -e "$@"
}

function debug() {
  [[ "$DEBUG" == "1" ]] && log "${YELLOW}DEBUG: $@${BLANK}"
}

function error() {
  log "${RED}ERROR: $@${BLANK}" 1>&2
}

function fatal() {
  log "${RED}FATAL: $@${BLANK}" 1>&2
  exit 1
}
