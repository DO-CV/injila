#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <iostream>

#include <boost/filesystem.hpp>

#include "DO/Injila/FileSystem/FileSystem.hpp"


using namespace std;
namespace fs = boost::filesystem;


namespace DO { namespace Injila {

	string basename(const string& filepath)
	{
    return fs::basename(filepath);
	}

	bool create_directory(const string& dirName)
	{
		if(fs::is_directory(dirName))
			return true;
		try
		{
			fs::create_directory(dirName);
		}
		catch(const std::exception & e)
		{
			cout << e.what() << endl;
			return false;
		}
		return true;
	}

	bool copy_file(const string& from, const string& to)
	{
		try
		{
			fs::copy_file(
				from, to,
				fs::copy_option::overwrite_if_exists
				);
		}
		catch (const std::exception & e)
		{
			cout << e.what() << endl;
			return false;
		}
		return true;
	}

  bool file_exists(const std::string& filename)
  {
    return fs::exists(filename);
  }

  vector<string> find_files(const string& folder,
                            const string& file_extension)
  {
    vector<string> files;

    fs::directory_iterator di(folder);
    fs::directory_iterator end_di;

    for (; di != end_di; ++di)
    {
      string ext = di->path().extension().string();
      transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

      if (ext == file_extension)
      {
        cout << di->path() << endl;
        files.push_back(di->path().string());
      }
    }

    return files;
  }

  vector<string> find_files_recursively(const string& folder,
                                        const string& file_extension,
                                        int num_files_max)
  {
    vector<string> files;

    fs::recursive_directory_iterator rdi(folder);
    fs::recursive_directory_iterator end_rdi;

    for (; rdi != end_rdi; ++rdi)
    {
      string ext = rdi->path().extension().string();
      transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

      if (ext == file_extension)
      {
        cout << rdi->path() << endl;
        files.push_back(rdi->path().string());
      }

      if (files.size() > static_cast<size_t>(num_files_max))
        break;
    }

    return files;
  }

} /* namespace Injila */
} /* namespace DO */
