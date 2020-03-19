#ifndef MARLIN_GEOMETRYPLUGIN_h
#define MARLIN_GEOMETRYPLUGIN_h 1

// -- std headers
#include <string>
#include <typeindex>

// -- marlin headers
#include "marlin/Exceptions.h"
#include "marlin/Component.h"

namespace marlin {

  class GeometryManager ;
  class Application ;

  /**
   *  @brief  GeometryPlugin class
   *  Responsible for loading geometry in Marlin and providing
   *  access to it through the GeometryManager
   */
  class GeometryPlugin : public Component {
    friend class GeometryManager ;

  public:
    GeometryPlugin() = delete ;
    virtual ~GeometryPlugin() = default ;
    GeometryPlugin(const GeometryPlugin &) = delete ;
    GeometryPlugin& operator=(const GeometryPlugin &) = delete ;

  protected:
    /**
     *  @brief  Constructor. To be called by sub classes
     *
     *  @param  gtype the geometry type
     */
    GeometryPlugin( const std::string &gtype ) ;

  protected:
    /**
     *  @brief  Load the geometry
     */
    virtual void loadGeometry() = 0 ;

    /**
     *  @brief  Get a handle on the geometry instance
     */
    virtual const void *handle() const = 0 ;

    /**
     *  @brief  Cleanup geometry
     */
    virtual void destroy() = 0 ;

  public:
    /**
     *  @brief  Get a type index object from the geometry handle
     */
    virtual std::type_index typeIndex() const = 0 ;

    /**
     *  @brief  Dump the geometry in the console
     */
    virtual void dumpGeometry() const = 0 ;

    /**
     *  @brief  Get the geometry description.
     *  Can be set by sub-classes in constructor (protected)
     */
    const std::string &description() const ;

    /**
     *  @brief  Get the geometry type
     */
    const std::string &type() const ;

    /**
     *  @brief  Print the complete geometry plugin description.
     */
    void print() const ;
    
  protected:
    /// Init the geometry plugin
    virtual void initComponent() override ; 
    
    /**
     *  @brief  Set the geometry description.
     *
     *  @param  desc a short plugin description
     */
    void setDescription( const std::string &desc ) ;

  protected:
    /// Whether to dump the geometry on creation
    BoolParameter       _dumpGeometry {*this, "DumpGeometry", "Whether to dump the geometry on creation", false} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  inline const std::string &GeometryPlugin::description() const {
    return componentDescription() ;
  }

  //--------------------------------------------------------------------------

  inline const std::string &GeometryPlugin::type() const {
    return componentName() ;
  }
  
  //--------------------------------------------------------------------------
  
  inline void GeometryPlugin::setDescription( const std::string &desc ) {
    setComponentDescription( desc ) ;
  }

} // end namespace marlin

#endif
