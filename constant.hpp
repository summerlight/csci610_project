#ifndef __CONSTANT_HPP__
#define __CONSTANT_HPP__

// Here, every parameters are given in a static number
// The only reason for this is to reduce cache miss as they're so frequently accessed... 
constexpr size_t MaxBlockCnt = 256;
constexpr size_t MaxPathBucket = 1024;
constexpr size_t MaxFuncCnt = 32;
constexpr size_t MaxThreadCnt = 4;
constexpr bool EnableLogging = true;

#endif // __CONSTANT_HPP__