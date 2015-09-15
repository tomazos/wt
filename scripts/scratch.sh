MAGNET=$1

cd `mktemp -d`
aria2c --bt-metadata-only=true --bt-save-metadata ${1}
transmission-show ${TORRENT}.torrent
ffmpeg -i ${INPUT} -vn -acodec opus -b:a 16000 -ar 12000 -ac 1 ${OUTPUT}.opus
