// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <algorithm>

#include "horace/query_string.h"

namespace horace {

static std::string decode(const std::string& encoded) {
	std::string decoded;
	decoded.reserve(encoded.length());

	size_t index = 0;
	while (index != encoded.length()) {
		size_t percent = encoded.find('%', index);
		if (percent == std::string::npos) {
			decoded.append(encoded, index);
			break;
		}
		decoded.append(encoded, index, percent - index);

		index = percent + 1;
		if ((index + 2 > encoded.length()) ||
			!isxdigit(encoded[index + 0]) ||
			!isxdigit(encoded[index + 1])) {

			decoded.append(1, '%');
			continue;
		}
		std::string hex = encoded.substr(index, 2);
		char c = static_cast<char>(stoi(hex, 0, 16));
		decoded.append(1, c);
		index += 2;
	}
	return decoded;
}

query_string::query_string(const std::string& query) {
	size_t index = 0;
	while (index != query.length()) {
		size_t param_end = query.find('&');
		if (param_end == std::string::npos) {
			param_end = query.length();
		}
		std::string param = query.substr(index, param_end - index);
		if (param_end = query.length()) {
			index = param_end;
		} else {
			index = param_end + 1;
		}

		if (!param.empty()) {
			std::string name(param);
			std::string value;
			size_t name_end = param.find('=');
			if (name_end != std::string::npos) {
				name = param.substr(0, name_end);
				value = param.substr(name_end + 1, std::string::npos);
			}
			std::replace(name.begin(), name.end(), '+', ' ');
			std::replace(value.begin(), value.end(), '+', ' ');
			name = decode(name);
			value = decode(value);
			_parameters[name] = value;
		}
	}
}

template<>
std::optional<std::string> query_string::find<std::string>(const std::string& name) const {
	auto f = _parameters.find(name);
	if (f == _parameters.end()) {
		return std::nullopt;
	}
	return f->second;
}

template<>
std::optional<long> query_string::find<long>(const std::string& name) const {
	auto f = _parameters.find(name);
	if (f == _parameters.end()) {
		return std::nullopt;
	}
	return stol(f->second);
}

template<>
std::optional<bool> query_string::find<bool>(const std::string& name) const {
	auto f = _parameters.find(name);
	if (f == _parameters.end()) {
		return std::nullopt;
	}
	if (f->second == "true") {
		return true;
	} else if (f->second == "false") {
		return false;
	} else {
		return std::nullopt;
	}
}

} /* namespace horace */
