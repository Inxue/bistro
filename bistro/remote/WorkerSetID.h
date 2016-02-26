/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include "bistro/bistro/if/gen-cpp2/common_types.h"

namespace facebook { namespace bistro {

namespace detail {
inline void addValueToHash(cpp2::SetHash* h, int64_t v) {
  h->xorAll = h->xorAll ^ v;
  h->addAll = h->addAll + v;
}
inline void removeValueFromHash(cpp2::SetHash* h, int64_t v) {
  h->xorAll = h->xorAll ^ v;
  h->addAll = h->addAll - v;
}
}

inline void addWorkerIDToHash(
    cpp2::WorkerSetHash* s,
    cpp2::BistroInstanceID id) {
  ++s->numWorkers;
  detail::addValueToHash(&s->startTime, id.startTime);
  detail::addValueToHash(&s->rand, id.rand);
}

inline void removeWorkerIDFromHash(
    cpp2::WorkerSetHash* s,
    cpp2::BistroInstanceID id) {
  CHECK_LT(0, s->numWorkers);
  --s->numWorkers;
  detail::removeValueFromHash(&s->startTime, id.startTime);
  detail::removeValueFromHash(&s->rand, id.rand);
}

/**
 * Less-than comparator for WorkerSetID history versions, assuming that a
 * and b are increment-only and can never be separated by more than 2^63 - 1
 * steps (that would require some extremely rapid worker turnover and a very
 * generous "worker lost" timeout).  Assumes two's complement.  Future: add
 * a static assert in case we're not compiled on a two's complement box.
 */
struct WorkerSetIDEarlierThan {
  bool operator()(int64_t a, int64_t b) const {
    if (a - b >= 0) {
      return false;
    } else if (b - a > 0) {
      return true;
    }
    LOG(FATAL) << "Versions differ by 2^63: a = " << a << ", b = " << b;
    return false;  // not reached but GCC isn't smart enough
  }
};

}}  // namespace facebook::bistro