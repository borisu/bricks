#pragma once

#define ASSERT_INITIATED if (!this->initiated) return BRICKS_INVALID_STATE
#define ASSERT_NOT_INITIATED if (this->initiated) return BRICKS_INVALID_STATE

#define ASSERT_STARTED if (!this->started) return BRICKS_INVALID_STATE
#define ASSERT_NOT_STARTED if (this->started) return BRICKS_INVALID_STATE

