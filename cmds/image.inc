. $SCRIPT_PATH/libs/colors.inc
. $SCRIPT_PATH/libs/core.inc

function kpish_image_commands() {
  echo "get rm ls rename"
}

function kpish_image_get_handler() {
  file=file:///mnt/downloads/ubuntu-server-20.04-arm64.tar.7z
  log "Downloading and extracting $(basename "$file") ..."
  image_get "$file"

  case $? in
    (1) error "Failed to download image archive.";;
    (2) error "Wrong image archive format.";;
    (3) error "Failed to extract image.";;
  esac
}

function kpish_image_rm_handler() {
  image_rm "$2"
}

function kpish_image_ls_handler() {
  local images=()
  image_ls images
  
  log "${LIGHT_WHITE}id\tname${BLANK}"

  for image in "${images[@]}"
  do
    local image_id=${image%%:*}
    local image_name=${image#*:}
    log "$image_id\t$image_name"
  done
}

function kpish_image_rename_handler() {
  image_rename "$2" "$3"
}
