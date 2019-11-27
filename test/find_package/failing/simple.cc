#include <marlin/PluginManager.h>

// Just for testing compilation and linking against Marlin libraries ...

int main() {
  auto &mgr = marlin::PluginManager::instance() ;
  mgr.dump() ;
  return 0;
}
