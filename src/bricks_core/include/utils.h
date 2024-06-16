#pragma once

#define ASSERT_INITIATED if (!this->initiated) return BRICKS_INVALID_STATE
#define ASSERT_NOT_INITIATED if (this->initiated) return BRICKS_INVALID_STATE

#define ASSERT_STARTED if (!this->started) return BRICKS_INVALID_STATE
#define ASSERT_NOT_STARTED if (this->started) return BRICKS_INVALID_STATE

#define ASSERT_PREINIT if (this->initiated || this->destroyed) return BRICKS_INVALID_STATE
#define ASSERT_READY if (!this->initiated || this->destroyed) return BRICKS_INVALID_STATE

#define BRICKS_MAP_PAIR(x) {#x,x}

#define SYNCHRONIZED(M) std::lock_guard lk__(M);

#define BRICKS_DEFAULT_CLIENT_TIMEOUT 1000