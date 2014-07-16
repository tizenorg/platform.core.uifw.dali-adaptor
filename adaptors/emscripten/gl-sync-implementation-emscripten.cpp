#include <gl/egl-sync-implementation.h>

using namespace Dali;

EglSyncObject::EglSyncObject( EglImplementation& eglSyncImpl )
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
  : mEglImplementation(NULL)
{
}

EglSyncImplementation::~EglSyncImplementation()
{
}

void EglSyncImplementation::Initialize( EglImplementation* /*impl*/ )
{
}

SyncObject* EglSyncImplementation::CreateSyncObject()
{
  return new SdlSyncObject();
}

void EglSyncImplementation::DestroySyncObject(SyncObject* /*syncObject*/)
{
  delete static_cast<SdlSyncObject*>( syncObject );
}

void EglSyncImplementation::InitializeEglSync()
{
}
