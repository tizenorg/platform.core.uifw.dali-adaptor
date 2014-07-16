#include <gl/egl-sync-implementation.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

namespace
{

class SdlSyncObject : public Integration::GlSyncAbstraction::SyncObject
{
public:
  /**
   * Constructor
   */
  SdlSyncObject() {}

  /**
   * Destructor
   */
  virtual ~SdlSyncObject() {}

  /**
   * @copydoc Dali::Integration::GlSyncAbstraction::SyncObject::IsSynced()
   */
  virtual bool IsSynced() {return true;};

private:
};

} // anon namespace

EglSyncObject::EglSyncObject( EglImplementation& /* eglSyncImpl */ )
//  : mEglImplementation( eglSyncImpl )
{
}

EglSyncObject::~EglSyncObject()
{
}

bool EglSyncObject::IsSynced()
{
  return true;
}

EglSyncImplementation::EglSyncImplementation()
//   : mEglImplementation(NULL)
{
}

EglSyncImplementation::~EglSyncImplementation()
{
}

void EglSyncImplementation::Initialize( EglImplementation* /*impl*/ )
{
}

Integration::GlSyncAbstraction::SyncObject* EglSyncImplementation::CreateSyncObject()
{
  return new SdlSyncObject();
}

void EglSyncImplementation::DestroySyncObject(SyncObject* syncObject)
{
  delete static_cast<SdlSyncObject*>( syncObject );
}

void EglSyncImplementation::InitializeEglSync()
{
}

} // namespace Adaptor
} // namespace Internal
} // namespace Dali
