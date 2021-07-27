. $SCRIPT_PATH/libs/logger.inc
. $SCRIPT_PATH/libs/utils.inc
. $SCRIPT_PATH/libs/colors.inc

KPISH_PROMPT="\x01$BOLD\x02\x01$GREEN\x02[kpictl]$ \x01$BLANK\x02"
KPISH_ARGS=()

function kpish_read() {
  read -e -p "$(echo -en "$KPISH_PROMPT")"
  local ifs=$IFS
  IFS=$'\n'
  KPISH_ARGS=($(xargs -n1 <<< "$REPLY"))
  IFS=$ifs
}

function kpish_handle() {
  shopt -s nocasematch
  local module="${1%_*}"
  [[ "$module" == "$1" ]] && {
    module="root"
  }
  local command="${1##*_}"

  local commands=""
  fn_exists "kpish_${module}_commands" && {
    commands=$(kpish_${module}_commands)
  }

  # debug "module = ${module}"
  # debug "command = $command"
  # debug "commands = $commands"
  # debug "args = ${@:2}"

  [[ "$module" == "root" && -z "$command" ]] && return

  [[ -n "$command" && "$command" != "?" ]] && {
    [[ " $commands " =~ " $command " ]] && {
      local fn="kpish_${module}_${command}_handler"
      fn_exists $fn || {
        fn="kpish_${command}_handler"
        fn_exists $fn
      } && kpish_exec $fn "$@" || kpish_unsafe_next $1 "${@:2}"
    } || {
      error "command not found."
    }
  }

  [[ -z "$command" || "$command" == "?" ]] && {
    [[ -n "$commands" ]] && {
      for cmd in $commands
      do
        log $cmd
      done
    } || {
      error "command not found."
    }
  }
  shopt -u nocasematch
}

function kpish_unsafe_next() {
  kpish_handle "$1_$2" "${@:3}" || true
}

function kpish_next() {
  [[ -n "$2" ]] && kpish_unsafe_next "$@"
}

function kpish_exec() {
  $1 "${@:2}" || true
}

function kpish_is_tlc() {
  local commands=""
  fn_exists "kpish_$1_commands" && {
    commands=$(kpish_$1_commands)
  }
  [[ -z "$2" || -z "$commands" || ! " $commands " =~ "$2" ]]
}
