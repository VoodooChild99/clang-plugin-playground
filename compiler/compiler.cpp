#include <unistd.h>

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/FormatVariadic.h"

using namespace llvm;

std::string plugin_dir;
std::vector<std::string> cc_params;
bool is_cxx = false;
bool has_src = false;

struct PluginInfo {
  std::string name;
  std::string path;
};

#define PLUGIN(NAME, PATH) \
  PluginInfo {    \
    .name = NAME, \
    .path = PATH, \
  }

const std::vector<PluginInfo> plugins = {
  PLUGIN("LAC", "libLACommenter.so"),
};

static inline void find_plugin_dir(const std::string &cc) {
  SmallString<128> path;
  auto err_code = sys::fs::real_path(cc, path, true);
  if (err_code) {
    errs() << "Failed to resolve real path for " << cc << "\n";
    exit(1);
  }
  auto slash_idx = path.find_last_of('/');
  if (slash_idx == StringRef::npos) {
    errs() << "Failed to locate plugin directory\n";
    exit(2);
  }
  plugin_dir = path.substr(0, slash_idx + 1).str() + "../lib";
}

static inline void check_cc(const std::string &cc) {
  auto slash_idx = cc.find_last_of('/');
  std::string name =
      (slash_idx == std::string::npos) ? cc : cc.substr(0, slash_idx);
  if (name == CUSTOM_CXX) {
    is_cxx = true;
  }
  cc_params.push_back(is_cxx ? CXX_COMPILER_BIN : C_COMPILER_BIN);
}

static inline bool is_suffix_c_cxx(const std::string &suffix) {
  return suffix == "c" || suffix == "cpp" || suffix == "cc";
}

static inline void check_src(const std::vector<std::string> &argv) {
  for (auto &arg : argv) {
    auto dot_indx = arg.find_last_of('.');
    if (dot_indx != std::string::npos) {
      auto suffix = arg.substr(dot_indx + 1);
      if (is_suffix_c_cxx(suffix)) {
        has_src = true;
        return;
      }
    }
  }
}

static inline void load_clang_plugin(const PluginInfo &plugin) {
  cc_params.push_back("-Xclang");
  cc_params.push_back("-load");
  cc_params.push_back("-Xclang");
  cc_params.push_back(formatv("{0}/{1}", plugin_dir, plugin.path));
  cc_params.push_back("-Xclang");
  cc_params.push_back("-add-plugin");
  cc_params.push_back("-Xclang");
  cc_params.push_back(plugin.name);
}

static void edit_params(const std::vector<std::string> &argv) {
  check_cc(argv[0]);
  check_src(argv);

  if (has_src) {
    // apply plugins
    for (auto &plugin : plugins) {
      load_clang_plugin(plugin);
    }
  }
  cc_params.insert(cc_params.end(), argv.begin() + 1, argv.end());
}

static int inline execvp_cxx(const std::vector<std::string> &argv) {
  std::vector<const char*> c_argv;
  for (auto &arg : argv) {
    c_argv.push_back(arg.c_str());
  }
  c_argv.push_back(NULL);
  return execvp(c_argv[0], (char *const *)c_argv.data());
}

int main(int argc, char *argv[]) {
  std::vector<std::string> _argv;
  for (int i = 0; i < argc; ++i) {
    _argv.push_back(std::string(argv[i]));
  }
  find_plugin_dir(_argv[0]);
  edit_params(_argv);

  int ret = execvp_cxx(cc_params);
  std::string err_msg;
  raw_string_ostream SO(err_msg);
  SO << "execvp failed: ";
  for (auto &arg : cc_params) {
    SO << arg << " ";
  }
  SO << ". Error: " << strerror(errno) << "\n";
  errs() << err_msg;
  return ret;
}