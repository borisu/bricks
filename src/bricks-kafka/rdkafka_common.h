#pragma once

#ifdef BRICKSKAFKA_EXPORTS
#define BRICKSKAFKA_API __declspec(dllexport)
#else
#define BRICKSKAFKA_API __declspec(dllimport)
#endif
