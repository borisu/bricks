#pragma once

#define ASSERT_INITIATED if (!this->initiated) return BRICKS_INVALID_STATE
#define ASSERT_NOT_INITIATED if (this->initiated) return BRICKS_INVALID_STATE

#define ASSERT_STARTED if (!this->started) return BRICKS_INVALID_STATE
#define ASSERT_NOT_STARTED if (this->started) return BRICKS_INVALID_STATE

#define ASSERT_PREINIT do { if (this->destroyed) return BRICKS_OBJECT_DESTROYED; if ( this->initiated) return BRICKS_INVALID_STATE; } while (false)
#define ASSERT_READY   do { if (this->destroyed) return BRICKS_OBJECT_DESTROYED; if (!this->initiated) return BRICKS_INVALID_STATE; } while (false)

#define BRICKS_MAP_PAIR(x) {#x,x}

#define SYNCHRONIZED(M) std::lock_guard lk__(M);

#define BRICKS_DEFAULT_CLIENT_TIMEOUT 1000