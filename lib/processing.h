#pragma once

#include "aggregate_by_key.h"
#include "as_data_flow.h"
#include "dir.h"
#include "drop_nullopt.h"
#include "filter.h"
#include "lazy_pipeline.h"
#include "open_files.h"
#include "out.h"
#include "split_expected.h"
#include "split.h"
#include "traits.h"
#include "transform.h"
#include "write.h"

#include <optional>

template <typename Key, typename Value>
struct KV {
	Key key;
	Value value;
};

template <typename Base, typename Joined>
struct JoinResult {
	Base base;
	std::optional<Joined> joined;
};
