import libtorrent
info = libtorrent.torrent_info('/data/torrents/000354081b5996b61e860295a6adda26f5fc8536.torrent')
for f in info.files():
    print "%s - %s" % (f.path, f.size)

