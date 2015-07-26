#pragma once

#include <string>
#include <vector>


namespace DO { namespace Injila {

	std::string basename(const std::string& filepath);

	bool create_directory(const std::string& dirName);

	bool copy_file(const std::string& from, const std::string& to);

  bool file_exists(const std::string& filename);

  std::vector<std::string> find_files(const std::string& folder,
                                      const std::string& file_extension);

  std::vector<std::string> find_files_recursively(const std::string& folder,
                                                  const std::string& file_extension,
                                                  int num_files_max = 10);

} /* namespace injila */
} /* namespace DO */
