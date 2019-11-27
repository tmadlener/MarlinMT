
// -- marlin headers
#include "marlin/GeometryPlugin.h"
#include "marlin/PluginManager.h"

namespace marlin {

  /**
   *  @brief  EmptyGeometry class
   *  Implement an empty geometry
   */
  class EmptyGeometry : public GeometryPlugin {
  public:
    EmptyGeometry(const EmptyGeometry &) = delete ;
    EmptyGeometry& operator=(const EmptyGeometry &) = delete ;

  public:
    EmptyGeometry() : GeometryPlugin( "Empty" ) { /* nop */ }

  protected:
    void loadGeometry() { /* nop */ }
    const void *handle() const { return nullptr ; }
    void destroy() { /* nop */ }
    std::type_index typeIndex() const { return std::type_index( typeid(nullptr) ) ; }
    void dumpGeometry() const { /* nop */ }
  };

  MARLIN_DECLARE_GEOPLUGIN( EmptyGeometry )
  
}