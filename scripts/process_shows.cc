#define BOOST_ASIO_DYN_LINK

#include <boost/filesystem.hpp>
#include <libtorrent/file_storage.hpp>
#include <libtorrent/torrent_info.hpp>

using namespace boost::filesystem;

int main() {
  path torrent_dir("/data/torrents");
  directory_iterator it(torrent_dir);
  for (; it != directory_iterator(); it++) {
    path torrent_file = *it;
    if (is_regular_file(torrent_file)) {
      libtorrent::torrent_info info(torrent_file.string());
      std::cout << "START_TORRENT: " << info.total_size() << " "
                << torrent_file.string() << std::endl;
      for (auto it = info.begin_files(); it != info.end_files(); it++) {
        std::cout << it->size << " " << it->filename() << std::endl;
      }
      std::cout << "END_TORRENT" << std::endl;
    }
  }
}
